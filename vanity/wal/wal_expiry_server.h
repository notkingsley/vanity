//
// Created by kingsli on 3/15/24.
//

#ifndef VANITY_WAL_EXPIRY_SERVER_H
#define VANITY_WAL_EXPIRY_SERVER_H

#include "db/servers/base_database_server.h"
#include "wal_write_server.h"


namespace vanity::wal {

/*
 * A WalExpiryServer ensures correctness of wal records
 * about keys that are being expired and vice versa during recovery
 */
class WalExpiryServer : public virtual BaseDatabaseServer, public virtual WalWriteServer
{
private:
	// registered for each database's pre-expire callback
	void on_expire(const std::string& key, uint db);

	// convenience function to deep_purge() all databases
	void deep_purge_all();

public:
	// prepare the database for a pending persist operation
	// this is like a best-effort to purge expired keys before
	// WAL is closed and synchronised with the databases
	// since the Database cannot deep_purge() internally during
	// persist (due to WAL being temporarily closed)
	void pre_persist();

	// set up the databases for WAL recovery
	// there should be no database interactions of any form
	// until this is called
	void pre_recovery();

	// finish up recovery to begin normal operations
	// also deep purges all databases, so WAL
	// should be open when called
	void post_recovery();

	// install on_expire hooks to all databases after
	// a possible load from file
	// this isn't done on construction because databases loaded
	// from a file are move constructed, replacing the ones that
	// were existing at construction
	void post_database_load();
};


} // namespace vanity::wal

#endif //VANITY_WAL_EXPIRY_SERVER_H
