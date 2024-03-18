//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_RECOVERY_SERVER_H
#define VANITY_WAL_RECOVERY_SERVER_H

#include <filesystem>
#include <fstream>

#include "db/servers/expiry_database_server.h"
#include "utils/serializer.h"
#include "wal_expiry_server.h"


namespace vanity::wal {

/*
 * A WalRecoveryServer recovers from a Write Ahead Log
 */
class WalRecoveryServer : public virtual ExpiryDatabaseServer, public virtual WalExpiryServer
{
public:
	// recover from a WAL
	void recover_from(const std::filesystem::path& wal_file);
};

} // namespace vanity::wal

#endif //VANITY_WAL_RECOVERY_SERVER_H
