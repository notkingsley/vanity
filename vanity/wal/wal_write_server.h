//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_WRITE_SERVER_H
#define VANITY_WAL_WRITE_SERVER_H

#include "db/db/types.h"
#include "session_server.h"
#include "write_ahead_logger.h"


namespace vanity::wal {

/*
 * A WalWriteServer writes to a Write Ahead Log
 */
class WalWriteServer : public virtual SessionServer
{
private:
	// the logger
	WriteAheadLogger m_logger;

public:
	// use file for the WAL
	void wal_to(const std::filesystem::path &wal_file);

	// log a transaction that's about to happen
	void wal_transaction(Client& client, const std::string& commands, size_t size);

	// obtain a reference to the mutex
	std::mutex &wal_mutex();

	// get the logger
	// TODO: remove
	WriteAheadLogger &wal_logger() { return m_logger; }
};

} // namespace vanity::wal

#endif //VANITY_WAL_WRITE_SERVER_H
