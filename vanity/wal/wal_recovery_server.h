//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_RECOVERY_SERVER_H
#define VANITY_WAL_RECOVERY_SERVER_H

#include <filesystem>
#include <fstream>

#include "persist_journal_server.h"
#include "transaction_server.h"
#include "utils/serializer.h"


namespace vanity::wal {

/*
 * A WalRecoveryServer recovers from a Write Ahead Log
 */
class WalRecoveryServer : public virtual PersistJournalServer, public virtual TransactionServer
{
private:
	// perform recovery from the wal_file
	void do_recover();

public:
	// create a WAL recovery server
	WalRecoveryServer();

	// recover previous state from the WAL file
	void recover();
};

} // namespace vanity::wal

#endif //VANITY_WAL_RECOVERY_SERVER_H
