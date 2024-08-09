//
// Created by kingsli on 10/29/23.
//

#include "base_database_server.h"

namespace vanity {

BaseDatabaseServer::BaseDatabaseServer(logger_type& logger) : m_databases(create_databases(logger)) {}

auto BaseDatabaseServer::database(Client &client) -> db_type& {
	return m_databases[session_db(client)];
}

void BaseDatabaseServer::request_switch_db(Client &client, int64_t db) {
	if (not validate_db_index(client, db))
		return;

	session_db(client) = db;
	send(client, ok());
}

void BaseDatabaseServer::request_get(Client &client, const std::string &key) {
	auto value = database(client).get(key);
	if (not value.has_value())
		return send(client, null());

	auto& data = value.value();
	switch (data.index()) {
		case 0:
			return send(client, ok(std::get<0>(data)));
		case 1:
			return send(client, ok(std::get<1>(data)));
		case 2:
			return send(client, ok(std::get<2>(data)));
		case 3:
			return send(client, ok(std::get<3>(data)));
		case 4:
			return send(client, ok(std::get<4>(data)));
		case 5:
			return send(client, ok(std::get<5>(data)));
		default:
			throw std::runtime_error("invalid type");
	}
}

void BaseDatabaseServer::request_del(Client &client, const std::string &key) {
	if (database(client).del(key))
		send(client, ok());
	else
		send(client, null());
}

void BaseDatabaseServer::request_type(Client &client, const std::string &key) {
	auto type = database(client).type(key);
	if (not type.has_value())
		return send(client, null());

	switch (type.value()) {
		case 0:
			return send(client, ok().serialize_type<db::db_index_t<0>>().move());
		case 1:
			return send(client, ok().serialize_type<db::db_index_t<1>>().move());
		case 2:
			return send(client, ok().serialize_type<db::db_index_t<2>>().move());
		case 3:
			return send(client, ok().serialize_type<db::db_index_t<3>>().move());
		case 4:
			return send(client, ok().serialize_type<db::db_index_t<4>>().move());
		case 5:
			return send(client, ok().serialize_type<db::db_index_t<5>>().move());
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

void BaseDatabaseServer::request_keys(Client &client) {
	auto keys = database(client).keys();
	if (keys.empty())
		return send(client, null());
	else
		send(client, ok(keys));
}

void BaseDatabaseServer::request_copy_to(Client &client, const std::string &from, const std::string &to) {
	if (database(client).copy_to(from, to))
		send(client, ok());
	else
		send(client, null());
}

void BaseDatabaseServer::request_move_to(Client &client, const std::string &from, const std::string &to) {
	if (database(client).move_to(from, to))
		send(client, ok());
	else
		send(client, null());
}

void BaseDatabaseServer::request_copy_to_db(Client &client, const std::string &from, int64_t dest) {
	if (not validate_db_index(client, dest))
		return;

	if (dest == session_db(client))
		return send(client, ok());

	if (database(client).copy_to_db(from, m_databases[dest]))
		send(client, ok());
	else
		send(client, null());
}

void BaseDatabaseServer::request_move_to_db(Client &client, const std::string &from, int64_t dest) {
	if (not validate_db_index(client, dest))
		return;

	if (dest == session_db(client))
		return send(client, ok());

	if (database(client).move_to_db(from, m_databases[dest]))
		send(client, ok());
	else
		send(client, null());
}

bool BaseDatabaseServer::validate_db_index(Client &client, int64_t index) {
	if (index < 0) {
		send(client, error(" db index must be non-negative"));
		return false;
	}

	if (index >= M_NUM_DATABASES) {
		send(client, error((" db index must be less than " + std::to_string(M_NUM_DATABASES)).c_str()));
		return false;
	}

	return true;
}

} // namespace vanity