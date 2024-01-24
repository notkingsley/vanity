//
// Created by kingsli on 12/4/23.
//

#include "auto_expiry_database_server.h"

namespace vanity {

AutoExpiryDatabaseServer::AutoExpiryDatabaseServer() {
	repeat(server_event::expire, M_EXPIRE_INTERVAL);
}

void AutoExpiryDatabaseServer::event_expire() {
	for (auto& db : m_databases)
		db.deep_purge();
}

} // namespace vanity