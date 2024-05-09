//
// Created by kingsli on 2/18/24.
//

#ifndef VANITY_PERSIST_JOURNAL_SERVER_H
#define VANITY_PERSIST_JOURNAL_SERVER_H

#include <utility>

#include "db/servers/base_database_server.h"
#include "wal_expiry_server.h"


namespace vanity {

namespace wal {

/*
 * A PersistJournalServer journals a persist operation to provide crash recovery
 */
class PersistJournalServer :
	public virtual BaseDatabaseServer,
	public virtual WalExpiryServer
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
