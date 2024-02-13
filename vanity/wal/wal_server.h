//
// Created by kingsli on 2/9/24.
//

#ifndef VANITY_WAL_SERVER_H
#define VANITY_WAL_SERVER_H

#include <optional>

#include "db/servers/database_server.h"
#include "wal_recovery_server.h"
#include "wal_write_server.h"


namespace vanity {

/*
 * A WALServer is a Write Ahead Log Server
 */
class WALServer :
	public virtual DatabaseServer,
	public virtual WALWriteServer,
	public virtual WALRecoveryServer
{
	// the WAL file, if any
	const std::optional<std::filesystem::path> m_wal_file;

public:
	// create a WAL server
	explicit WALServer(const std::optional<std::filesystem::path>& wal_file);

	// recover previous state from the WAL file
	void recover();
};

} // namespace vanity

#endif //VANITY_WAL_SERVER_H
