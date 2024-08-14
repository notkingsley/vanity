//
// Created by kingsli on 8/14/24.
//

#include "database_object_server.h"

namespace vanity {

DatabaseObjectServer::DatabaseObjectServer() : m_databases(create_databases()) {}

auto DatabaseObjectServer::wal_logger() -> wal_logger_t& {
	return m_wal_logger;
}

auto DatabaseObjectServer::database(Client &client) -> db_type& {
	return m_databases[session_db(client)];
}

auto DatabaseObjectServer::database(size_t index) -> db_type& {
	return m_databases[index];
}

void DatabaseObjectServer::deep_purge_databases() {
	for (auto& db: m_databases)
		db.deep_purge();
}

void DatabaseObjectServer::enable_databases_expiry(bool enable) {
	for (auto& db: m_databases)
		db.expiry_enabled(enable);
}

} // namespace vanity
