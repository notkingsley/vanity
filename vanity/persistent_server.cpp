//
// Created by kingsli on 10/3/23.
//

#include "persistent_server.h"

namespace vanity{

PersistentServer::PersistentServer() noexcept {
	if (m_db_file)
		repeat(server_event::persist, M_PERSIST_INTERVAL);
}

bool PersistentServer::persist() {
	if (!m_db_file)
		return false;

	PersistJournalServer::persist_no_check();
	logger().info("Persisted databases to " + m_db_file.value().string());
	return true;
}

void PersistentServer::request_persist(Client & client) {
	if (persist())
		send(client, ok());
	else
		send(client, error("Persistence disabled"));
}

void PersistentServer::stop() {
	persist();
}

void PersistentServer::event_persist() {
	persist();
}

} // namespace vanity