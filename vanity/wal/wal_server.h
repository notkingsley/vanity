//
// Created by kingsli on 2/9/24.
//

#ifndef VANITY_WAL_SERVER_H
#define VANITY_WAL_SERVER_H

#include <optional>

#include "persist_journal_server.h"
#include "wal_recovery_server.h"


namespace vanity {

namespace wal {

/*
 * A WalServer is a Write Ahead Log Server
 */
class WalServer :
	public virtual PersistJournalServer,
	public virtual WalRecoveryServer
{
public:
	// create a WAL server
	explicit WalServer();

	// recover previous state from the WAL file
	void recover();
};

} // namespace wal

using wal::WalServer;

} // namespace vanity

#endif //VANITY_WAL_SERVER_H
