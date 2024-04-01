//
// Created by kingsli on 2/18/24.
//

#include "persist_journal_server.h"

namespace vanity::wal {

// return a path with the same parent as file, but with the given prefix
static auto with_name_prefix(const std::filesystem::path& file, const char* prefix) {
	return file.parent_path() / (prefix + file.filename().string());
}

Journalist::path Journalist::make_temp_db_file(const path &db_file) {
	return with_name_prefix(db_file, "tmp.");
}

Journalist::path Journalist::make_old_db_file(const path &db_file) {
	return with_name_prefix(db_file, "old.");
}

void Journalist::journal_db_file_no_exist() {
	auto no_exist = m_empty_journal.db_file_no_exist();
	rename(m_tmp_db_file, m_db_file);
	remove(m_wal_file);
	no_exist.delete_();
}

void Journalist::journal_db_file_exist() {
	auto old_db_file = make_old_db_file(m_db_file);
	auto exist = m_empty_journal.db_file_exist();
	auto moving = exist.moving_existing_db_file(old_db_file);
	rename(m_db_file, old_db_file);
	auto moved = moving.moved_existing_db_file();
	rename(m_tmp_db_file, m_db_file);
	auto moved_new = moved.moved_new_db_file();
	remove(m_wal_file);
	moved_new.delete_();
	remove(old_db_file);
}

Journalist::Journalist(const path &journal_file, const path &db_file, const path &wal_file):
	m_db_file{db_file},
	m_tmp_db_file{make_temp_db_file(db_file)},
	m_wal_file{wal_file},
	m_empty_journal{journal::new_journal_maker(journal_file)}
{}

void Journalist::switch_and_journal() {
	if (exists(m_db_file))
		journal_db_file_exist();
	else
		journal_db_file_no_exist();
}

const Journalist::path &Journalist::tmp_db_file() const {
	return m_tmp_db_file;
}


void PersistJournalServer::do_persist(const path &file) {
	std::ofstream out{file, std::ios::binary};
	for (auto& db : m_databases)
		db.persist(out);
}

auto PersistJournalServer::lock_all() {
	return [this]<size_t... I>(std::index_sequence<I...>) {
		return std::scoped_lock { wal_mutex(), m_databases[I].mutex()... };
	}(std::make_index_sequence<M_NUM_DATABASES>{});
}

void PersistJournalServer::persist_with_wal() {
	ClosedWal closed_wal {*this, *m_wal_file};
	auto lock {lock_all()};
	Journalist journalist {*m_journal_file, *m_db_file, *m_wal_file};
	do_persist(journalist.tmp_db_file());
	journalist.switch_and_journal();
}

void PersistJournalServer::persist_without_wal() {
	auto tmp = with_name_prefix(*m_db_file, "tmp.");
	do_persist(tmp);
	rename(tmp, *m_db_file);
}

void PersistJournalServer::load_databases() {
	if (not m_db_file or not exists(*m_db_file))
		return;

	std::ifstream in{*m_db_file, std::ios::binary};
	for (auto& db : m_databases)
		db = db::Database::from(in);
}

void PersistJournalServer::pre_database_load() {
	if (not m_wal_file or not m_db_file)
		return;

	journal::RecoveredJournal recovered_journal{*m_journal_file};
	switch (recovered_journal.get_state()) {
		case journal::JournalState::EMPTY_JOURNAL:
		{
			break;
		}
		case journal::JournalState::DB_FILE_NO_EXIST:
		{
			if (exists(*m_db_file))
				remove(*m_wal_file);
			break;
		}
		case journal::JournalState::DB_FILE_EXIST:
		{
			break;
		}
		case journal::JournalState::MOVING_EXISTING_DB_FILE:
		{
			const auto& old = recovered_journal.get_existing_db_file();
			if (exists(old) and exists(*m_db_file))
				throw std::runtime_error("Journal violation: both old and current db files exist");

			if (exists(*m_db_file))
				break;

			if (not exists(old))
				throw std::runtime_error("Journal violation: none of old or current db files exist");

			rename(old, *m_db_file);
			break;
		}
		case journal::JournalState::MOVED_EXISTING_DB_FILE:
		{
			const auto& old = recovered_journal.get_existing_db_file();
			rename(old, *m_db_file);
			break;
		}
		case journal::JournalState::MOVED_NEW_DB_FILE:
		{
			remove(*m_wal_file);
			break;
		}
	}
}

PersistJournalServer::PersistJournalServer(optional_path wal_file, optional_path db_file, optional_path journal_file):
	m_wal_file{std::move(wal_file)},
	m_db_file{std::move(db_file)},
	m_journal_file{std::move(journal_file)}
{
	if (m_wal_file and m_db_file and not m_journal_file)
		throw std::invalid_argument("Journal file must be provided if WAL and DB files are provided");

	pre_database_load();
	load_databases();
	post_database_load();
}

void PersistJournalServer::persist_no_check() {
	pre_persist();

	if (m_wal_file and m_db_file)
		persist_with_wal();
	else
		persist_without_wal();
}

} // namespace vanity::wal
