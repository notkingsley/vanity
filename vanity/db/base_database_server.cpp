//
// Created by kingsli on 10/29/23.
//

#include "base_database_server.h"
#include "serialize.h"

namespace vanity {

void BaseDatabaseServer::start() {
	for (auto &database : m_databases)
		database.start(M_MAX_TIMEOUT * 1000 * 1000);
}

void BaseDatabaseServer::stop() {
	for (auto &database : m_databases)
		database.stop();
}

db::SerialDatabase& BaseDatabaseServer::database(Client &client) {
	return m_databases[client.db()];
}

void BaseDatabaseServer::request_del(Client &client, const std::string &key) {
	if (database(client).del(key))
		send(client, ok());
	else
		send(client, error());
}

void BaseDatabaseServer::request_type(Client &client, const std::string &key) {
	auto type = database(client).type(key);
	if (not type.has_value())
		return send(client, null());

	switch (type.value()) {
		case 0:
			return send(client, ok(serialize_type<db::db_index_t<0>>()));
		case 1:
			return send(client, ok(serialize_type<db::db_index_t<1>>()));
		case 2:
			return send(client, ok(serialize_type<db::db_index_t<2>>()));
		case 3:
			return send(client, ok(serialize_type<db::db_index_t<3>>()));
		default:
			throw std::runtime_error("invalid type");
	}
}

void BaseDatabaseServer::request_reset(Client &client) {
	database(client).reset();
	logger().info("Reset database");
	send(client, ok());
}

void BaseDatabaseServer::request_exists(Client &client, const std::string &key) {
	if (database(client).has(key))
		send(client, ok());
	else
		send(client, null());
}

} // namespace vanity