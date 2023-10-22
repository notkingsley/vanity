//
// Created by kingsli on 9/14/23.
//

#include "database_server.h"


namespace vanity {

void DatabaseServer::start() {
	for (auto &database : m_databases)
		database.start(M_MAX_TIMEOUT);
}

void DatabaseServer::stop() {
	for (auto &database : m_databases)
		database.stop();
}

db::SerialDatabase& DatabaseServer::database(Client &client) {
	return m_databases[client.db()];
}

void DatabaseServer::request_get(Client &client, const std::string &key) {
	auto value = database(client).get(key);
	if (not value.has_value())
		return send_null(client);

	auto data = value.value();
	switch (data.index()) {
		case 0:
			return send_ok(client, prepare(std::get<0>(data)));
		case 1:
			return send_ok(client, prepare(std::get<1>(data)));
		case 2:
			return send_ok(client, prepare(std::get<2>(data)));
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::request_set(Client &client, const std::string &key, const std::string &value) {
	database(client).set(key, value);
	send_ok(client);
}

void DatabaseServer::request_set(Client &client, const std::string &key, const int64_t &value) {
	database(client).set(key, value);
	send_ok(client);
}

void DatabaseServer::request_set(Client &client, const std::string &key, const double &value) {
	database(client).set(key, value);
	send_ok(client);
}

void DatabaseServer::request_del(Client &client, const std::string &key) {
	if (database(client).del(key))
		send_ok(client);
	else
		send_error(client);
}

void DatabaseServer::request_type(Client &client, const std::string &key) {
	auto type = database(client).type(key);
	if (not type.has_value())
		return send_null(client);

	switch (type.value()) {
		case 0:
			return send_ok(client, type_to_string<db::db_index_t<0>>::value);
		case 1:
			return send_ok(client, type_to_string<db::db_index_t<1>>::value);
		case 2:
			return send_ok(client, type_to_string<db::db_index_t<2>>::value);
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::request_reset(Client &client) {
	database(client).reset();
	logger().info("Reset database");
	send_ok(client);
}

} // namespace vanity
