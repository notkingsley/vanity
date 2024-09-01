//
// Created by kingsli on 8/14/24.
//

#include "database_object_server.h"

namespace vanity {

DatabaseObjectServer::DatabaseObjectServer() : m_databases(create_databases()) {}

auto DatabaseObjectServer::wal_logger() -> wal_logger_t& {
	return m_wal_logger;
}

auto DatabaseObjectServer::database(Client &client) -> db_wrapper_type {
	return db_wrapper_type{database_obj(client), session_trn_id(client)};
}

auto DatabaseObjectServer::database_obj(Client &client) -> db_type& {
	return database_obj(session_db(client));
}

auto DatabaseObjectServer::database_obj(size_t index) -> db_type& {
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
