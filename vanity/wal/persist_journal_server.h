//
// Created by kingsli on 2/18/24.
//

#ifndef VANITY_PERSIST_JOURNAL_SERVER_H
#define VANITY_PERSIST_JOURNAL_SERVER_H

#include <utility>

#include "db/servers/base_database_server.h"
#include "journal.h"
#include "wal_expiry_server.h"
#include "wal_write_server.h"


namespace vanity {

namespace wal {

/*
 * An uncreative name, to tell the truth.
 * A Journalist handles the logic for journaling a persist operation
 * This is one-use only: one Journalist for one persist operation
 *
 * You create a Journalist, perform the persist operation, and then call switch_and_journal()
 */
class Journalist
{
private:
	using path = std::filesystem::path;

	using empty_journal_t = std::invoke_result_t<decltype(journal::new_journal_maker), path>;

	// the database file
	const path &m_db_file;

	// the temporary database file
	path m_tmp_db_file;

	// the WAL file
	const path &m_wal_file;

	// the empty journal
	empty_journal_t m_empty_journal;

	// return the path to a temporary database file
	// the path returned must be distinct from db_file
	static path make_temp_db_file(const path &db_file);

	// return the path to the old database file
	// the path returned must be distinct from db_file
	static path make_old_db_file(const path &db_file);

	// journal when the database file does not exist
	void journal_db_file_no_exist();

	// journal when the database file already exists
	void journal_db_file_exist();

public:
	// create a journalist
	Journalist(const path &journal_file, const path &db_file, const path &wal_file);

	// journal the persist operation
	void switch_and_journal();

	// get the temporary database file
	const path &tmp_db_file() const;
};

/*
 * A PersistJournalServer journals a persist operation to provide crash recovery
 */
class PersistJournalServer :
	public virtual BaseDatabaseServer,
	public virtual WalExpiryServer,
	public virtual WalWriteServer
{
private:
	using path = std::filesystem::path;

	using optional_path = std::optional<path>;

protected:
	// the WAL file, if any
	const optional_path m_wal_file;

	// the database file, if any
	const optional_path m_db_file;

	// the journal file, if any
	const optional_path m_journal_file;

private:
	// perform the persist operation into file
	void do_persist(const path &file);

	// perform a persist operation with WAL enabled (journaling the operation)
	// assumes the WAL file, the database file, and the journal file are all present
	void persist_with_wal();

	// persist the database without WAL enabled
	void persist_without_wal();

	// load the databases from the database file
	void load_databases();

	// recover from a possible crash using the journal
	// this should be called before any of db_file, wal_file, or journal_file are accessed
	// it ensures that the database file and the WAL are in a consistent state
	void pre_database_load();

	// return a lock on the wal mutex and all databases
	auto lock_all();

public:
	// create a PersistJournalServer
	PersistJournalServer(optional_path wal_file, optional_path db_file, optional_path journal_file);

	// persist the databases
	// assumes the database file is present
	void persist_no_check();
};

} // namespace wal

using wal::PersistJournalServer;

} // namespace vanity

#endif //VANITY_PERSIST_JOURNAL_SERVER_H
