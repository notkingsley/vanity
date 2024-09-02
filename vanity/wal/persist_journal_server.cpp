//
// Created by kingsli on 2/18/24.
//

#include "journalist.h"
#include "persist_journal_server.h"


namespace vanity::wal {

// return a path with the same parent as file, but with the given prefix
// defined in journalist.cpp
std::filesystem::path with_name_prefix(const std::filesystem::path& file, const char* prefix);


void PersistJournalServer::do_persist(const path &file) {
	std::ofstream out{file, std::ios::binary};
	for (auto& db : m_databases)
		db.persist(out);
}

auto PersistJournalServer::lock_all() {
	return [this]<size_t... I>(std::index_sequence<I...>) {
		return std::scoped_lock {database_obj(I).mutex()... };
	}(std::make_index_sequence<M_NUM_DATABASES>{});
}

void PersistJournalServer::persist_with_wal() {
	WriteAheadLogger::Closed closed {wal_logger(), *m_wal_file};
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
				throw WALError("Journal violation: both old and current db files exist");

			if (exists(*m_db_file))
				break;

			if (not exists(old))
				throw WALError("Journal violation: none of old or current db files exist");

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

	if (m_wal_file)
		wal_logger().wal_to(*m_wal_file);
}

void PersistJournalServer::persist_no_check() {
	deep_purge_databases();

	if (m_wal_file and m_db_file)
		persist_with_wal();
	else
		persist_without_wal();
}

} // namespace vanity::wal
