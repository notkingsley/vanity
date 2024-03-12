//
// Created by kingsli on 2/9/24.
//

#ifndef VANITY_WAL_SERVER_H
#define VANITY_WAL_SERVER_H

#include <optional>

#include "persist_journal_server.h"
#include "wal_recovery_server.h"


namespace vanity {

/*
 * A WALServer is a Write Ahead Log Server
 */
class WALServer :
	public virtual PersistJournalServer,
	public virtual WALRecoveryServer
{
public:
	// create a WAL server
	explicit WALServer();

	// recover previous state from the WAL file
	void recover();
};

} // namespace vanity

#endif //VANITY_WAL_SERVER_H
