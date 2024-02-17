//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_RECOVERY_SERVER_H
#define VANITY_WAL_RECOVERY_SERVER_H

#include <filesystem>
#include <fstream>

#include "utils/serializer.h"
#include "db/servers/base_database_server.h"
#include "wal_recovery_client.h"


namespace vanity {

/*
 * A WALRecoveryServer recovers from a Write Ahead Log
 */
class WALRecoveryServer : public virtual BaseDatabaseServer
{
public:
	// recover from a WAL
	void recover_from(const std::filesystem::path& wal_file);
};

} // namespace vanity

#endif //VANITY_WAL_RECOVERY_SERVER_H