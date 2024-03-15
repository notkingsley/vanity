//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_WRITE_SERVER_H
#define VANITY_WAL_WRITE_SERVER_H

#include <filesystem>
#include <fstream>
#include <utility>

#include "utils/serializer.h"
#include "session_server.h"


namespace vanity {

namespace wal {

/*
 * A WalWriteServer writes to a Write Ahead Log
 */
class WalWriteServer : public virtual SessionServer {
private:
	// the WAL file
	std::ofstream m_wal_file;

	// mutex for the WAL
	std::mutex m_wal_mutex;

public:
	// use file for the WAL
	void wal_to(const std::filesystem::path &wal_file);

	// close the WAL
	void close_wal();

	// log a request that's about to happen
	// requires op to be the operation extracted from the request
	void wal_request(Client &client, operation_t op, const std::string_view &request);

	// log an expiry that's about to happen
	void wal_expiry(const std::string &key, uint db);

	// obtain a reference to the mutex
	std::mutex &wal_mutex();
};

/*
 * RAII mechanism to close and reopen a WalWriteServer
 */
class ClosedWAL {
private:
	// the WalWriteServer
	WalWriteServer &m_wal;

	// the WAL file to reopen
	std::filesystem::path m_wal_file;

public:
	// close the WAL
	explicit ClosedWAL(WalWriteServer &wal, std::filesystem::path wal_file);

	// reopen the WAL
	~ClosedWAL();
};

} // namespace wal

using wal::WalWriteServer;

} // namespace vanity

#endif //VANITY_WAL_WRITE_SERVER_H
