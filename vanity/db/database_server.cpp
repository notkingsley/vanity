//
// Created by kingsli on 9/14/23.
//

#include "database_server.h"


namespace vanity {

void DatabaseServer::request_get(const Client &client, const std::string &key) {
	if (!m_database.has(key))
		return send(client, server_constants::null);

	auto value = m_database.get(key);
	switch (value.index()) {
		case 0:
			return send(client, RequestServer::prepare(std::get<0>(value)));
		case 1:
			return send(client, RequestServer::prepare(std::get<1>(value)));
		case 2:
			return send(client, RequestServer::prepare(std::get<2>(value)));
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::request_set(const Client &client, const std::string &key, const std::string &value) {
	m_database.set(key, value);
	send(client, server_constants::ok);
}

void DatabaseServer::request_set(const Client &client, const std::string &key, const int64_t &value) {
	m_database.set(key, value);
	send(client, server_constants::ok);
}

void DatabaseServer::request_set(const Client &client, const std::string &key, const double &value) {
	m_database.set(key, value);
	send(client, server_constants::ok);
}

void DatabaseServer::request_del(const Client &client, const std::string &key) {
	if (m_database.del(key))
		send(client, server_constants::ok);
	else
		send(client, server_constants::error);
}

void DatabaseServer::request_reset(const Client &client) {
	m_database.reset();
	logger().info("Reset database");
	send(client, server_constants::ok);
}

} // namespace vanity
