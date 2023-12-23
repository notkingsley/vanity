//
// Created by kingsli on 12/4/23.
//

#include "auto_expiry_database_server.h"

namespace vanity {

void AutoExpiryDatabaseServer::start() {
	m_stopped.clear();
	m_expire_loop_thread = std::thread(&AutoExpiryDatabaseServer::expire_loop, this);
}

void AutoExpiryDatabaseServer::stop() {
	m_stopped.set();
	m_expire_loop_thread.join();
}

void AutoExpiryDatabaseServer::expire() {
	for (auto& db : m_databases)
		db.deep_purge();
}

void AutoExpiryDatabaseServer::expire_loop() {
	while (not m_stopped.wait(M_EXPIRE_INTERVAL))
		push_event(server_event::expire);
}

void AutoExpiryDatabaseServer::event_expire() {
	expire();
}

} // namespace vanity