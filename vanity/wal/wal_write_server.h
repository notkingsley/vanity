//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_WRITE_SERVER_H
#define VANITY_WAL_WRITE_SERVER_H

#include <filesystem>
#include <fstream>

#include "utils/serializer.h"
#include "session_server.h"


namespace vanity {

/*
 * A WALWriteServer writes to a Write Ahead Log
 */
class WALWriteServer : public virtual SessionServer
{
private:
	// the WAL file
	std::ofstream m_wal_file;

	// mutex for the WAL
	std::mutex m_wal_mutex;

public:
	// use file for the WAL
	void wal_to(const std::filesystem::path& wal_file);

	// close the WAL
	void close_wal();

	// log an operation that's about to happen
	// requires op to be the operation extracted from the request
	void write_ahead(Client& client, operation_t op, const std::string_view& request);
};

} // namespace vanity

#endif //VANITY_WAL_WRITE_SERVER_H
