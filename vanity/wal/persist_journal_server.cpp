//
// Created by kingsli on 2/18/24.
//

#include "persist_journal_server.h"

namespace vanity {

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

void Journalist::clear_wal() {
	std::filesystem::remove(m_wal_file);
}

void Journalist::journal_db_file_no_exist() {
	auto no_exist_journal = m_empty_journal.journal_db_file_no_exist();
	std::filesystem::rename(m_tmp_db_file, m_db_file);
	clear_wal();
	no_exist_journal.journal_delete();
}

void Journalist::journal_db_file_exist() {
	auto old_db_file = make_old_db_file(m_db_file);
	auto exist_journal = m_empty_journal.journal_db_file_exist();
	auto moving_journal = exist_journal.journal_moving_existing_db_file(old_db_file);
	std::filesystem::rename(m_db_file, old_db_file);
	auto moved_journal = moving_journal.journal_moved_existing_db_file();
	std::filesystem::rename(m_tmp_db_file, m_db_file);
	auto moved_new_journal = moved_journal.journal_moved_new_db_file();
	clear_wal();
	moved_new_journal.journal_delete();
	std::filesystem::remove(old_db_file);
}

Journalist::Journalist(const path &journal_file, const path &db_file, const path &wal_file):
	m_db_file{db_file},
	m_tmp_db_file{make_temp_db_file(db_file)},
	m_wal_file{wal_file},
	m_empty_journal{journal::new_journal_maker(journal_file)}
{}

void Journalist::switch_and_journal() {
	if (std::filesystem::exists(m_db_file))
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

void PersistJournalServer::persist_with_wal() {
	ClosedWAL closed_wal{*this, *m_wal_file};
	{
		std::scoped_lock lock{
			wal_mutex(),
			m_databases[0].mutex(),
			m_databases[1].mutex(),
			m_databases[2].mutex(),
			m_databases[3].mutex(),
			m_databases[4].mutex(),
			m_databases[5].mutex(),
			m_databases[6].mutex(),
			m_databases[7].mutex(),
			m_databases[8].mutex(),
			m_databases[9].mutex(),
			m_databases[10].mutex(),
			m_databases[11].mutex(),
			m_databases[12].mutex(),
			m_databases[13].mutex(),
			m_databases[14].mutex(),
			m_databases[15].mutex()
		};
		Journalist journalist{*m_journal_file, *m_db_file, *m_wal_file};
		do_persist(journalist.tmp_db_file());
		journalist.switch_and_journal();
	}
}

void PersistJournalServer::persist_without_wal() {
	auto tmp = with_name_prefix(*m_db_file, "tmp.");
	do_persist(tmp);
	std::filesystem::rename(tmp, m_db_file.value());
}

void PersistJournalServer::load_databases() {
	if (m_db_file and std::filesystem::exists(*m_db_file)) {
		std::ifstream in{*m_db_file, std::ios::binary};
		for (auto& db : m_databases)
			db = db::Database::from(in);
		in.close();
	}
}

void PersistJournalServer::pre_recover_with_wal() {
	namespace fs = std::filesystem;
	journal::RecoveredJournal recovered_journal{*m_journal_file};

	switch (recovered_journal.get_state()) {
		case journal::JournalState::EMPTY_JOURNAL:
		{
			break;
		}
		case journal::JournalState::DB_FILE_NO_EXIST:
		{
			if (fs::exists(*m_db_file))
				fs::remove(*m_wal_file);
			break;
		}
		case journal::JournalState::DB_FILE_EXIST:
		{
			break;
		}
		case journal::JournalState::MOVING_EXISTING_DB_FILE:
		{
			const auto& old = recovered_journal.get_existing_db_file();
			if (fs::exists(old) and fs::exists(*m_db_file))
				throw std::runtime_error("Journal violation: both old and current db files exist");

			if (fs::exists(*m_db_file))
				break;

			if (not fs::exists(old))
				throw std::runtime_error("Journal violation: none of old or current db files exist");

			fs::rename(old, *m_db_file);
			break;
		}
		case journal::JournalState::MOVED_EXISTING_DB_FILE:
		{
			const auto& old = recovered_journal.get_existing_db_file();
			fs::rename(old, *m_db_file);
			break;
		}
		case journal::JournalState::MOVED_NEW_DB_FILE:
		{
			fs::remove(*m_wal_file);
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

	if (m_wal_file and m_db_file)
		pre_recover_with_wal();

	load_databases();
}

void PersistJournalServer::persist_no_check() {
	if (m_wal_file and m_db_file)
		persist_with_wal();
	else
		persist_without_wal();
}

} // namespace vanity
