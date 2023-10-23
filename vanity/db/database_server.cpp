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

void DatabaseServer::request_exists(Client &client, const std::string &key) {
	if (database(client).has(key))
		send_ok(client);
	else
		send_null(client);
}

void DatabaseServer::request_incr_int(Client &client, const std::string &key, db::int_t value) {
	auto result = database(client).incr_int(key, value);
	if (not result.has_value())
		return send_bad_type(client);

	send_ok(client, prepare(result.value()));
}

void DatabaseServer::request_incr_float(Client &client, const std::string &key, db::float_t value) {
	auto result = database(client).incr_float(key, value);
	if (not result.has_value())
		return send_bad_type(client);

	send_ok(client, prepare(result.value()));
}

void DatabaseServer::request_len_str(Client &client, const std::string &key) {
	auto result = database(client).len_str(key);
	if (not result.has_value())
		return send_bad_type(client);

	send_ok(client, prepare(result.value()));
}

} // namespace vanity
