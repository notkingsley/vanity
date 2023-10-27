//
// Created by kingsli on 9/14/23.
//

#include "database_server.h"


namespace vanity {

void DatabaseServer::start() {
	for (auto &database : m_databases)
		database.start(M_MAX_TIMEOUT * 1000 * 1000);
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
		return send(client, null());

	auto data = value.value();
	switch (data.index()) {
		case 0:
			return send(client, ok(serialize(std::get<0>(data))));
		case 1:
			return send(client, ok(serialize(std::get<1>(data))));
		case 2:
			return send(client, ok(serialize(std::get<2>(data))));
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::request_set(Client &client, const std::string &key, const std::string &value) {
	database(client).set(key, value);
	send(client, ok());
}

void DatabaseServer::request_set(Client &client, const std::string &key, const int64_t &value) {
	database(client).set(key, value);
	send(client, ok());
}

void DatabaseServer::request_set(Client &client, const std::string &key, const double &value) {
	database(client).set(key, value);
	send(client, ok());
}

void DatabaseServer::request_del(Client &client, const std::string &key) {
	if (database(client).del(key))
		send(client, ok());
	else
		send(client, error());
}

void DatabaseServer::request_type(Client &client, const std::string &key) {
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
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::request_reset(Client &client) {
	database(client).reset();
	logger().info("Reset database");
	send(client, ok());
}

void DatabaseServer::request_exists(Client &client, const std::string &key) {
	if (database(client).has(key))
		send(client, ok());
	else
		send(client, null());
}

void DatabaseServer::request_incr_int(Client &client, const std::string &key, db::int_t value) {
	auto result = database(client).incr_int(key, value);

	if (result.has_value())
		send(client, ok(serialize(result.value())));
	else
		send(client, bad_type());
}

void DatabaseServer::request_incr_float(Client &client, const std::string &key, db::float_t value) {
	auto result = database(client).incr_float(key, value);

	if (result.has_value())
		send(client, ok(serialize(result.value())));
	else
		send(client, bad_type());
}

void DatabaseServer::request_len_str(Client &client, const std::string &key) {
	auto result = database(client).len_str(key);

	if (result.has_value())
		send(client, ok(serialize(result.value())));
	else
		send(client, bad_type());
}

} // namespace vanity
