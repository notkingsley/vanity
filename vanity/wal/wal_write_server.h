//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_WRITE_SERVER_H
#define VANITY_WAL_WRITE_SERVER_H

#include "db/db/types.h"
#include "session_server.h"
#include "write_ahead_logger.h"


namespace vanity {

namespace wal {

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

	// log a request that's about to happen
	// requires op to be the operation extracted from the request
	void wal_request(Client &client, operation_t op, const std::string_view &request);

	// log an expiry that's about to happen
	void wal_expiry(const std::string &key, uint db);

	// log a set_expiry operation that's about to happen
	// this is WAL-ed separately because the replay of SET_EXPIRY
	// with a WAL is dependent on a time factor,
	// violating the invariance on the principle of
	// WAL: we must arrive at exactly the same state after
	// redoing each entry in the WAL
	void wal_set_expiry(const std::string& key, uint db, db::time_t expiry_time);

	// log a transaction that's about to happen
	void wal_transaction(Client& client, const std::string& commands, size_t size);

	// obtain a reference to the mutex
	std::mutex &wal_mutex();

	// get the logger
	// TODO: remove
	WriteAheadLogger &wal_logger() { return m_logger; }
};

} // namespace wal

using wal::WalWriteServer;

} // namespace vanity

#endif //VANITY_WAL_WRITE_SERVER_H
