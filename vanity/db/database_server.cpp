//
// Created by kingsli on 9/14/23.
//

#include "database_server.h"


namespace vanity {

void DatabaseServer::start() {
	m_database.start(M_MAX_TIMEOUT);
}

void DatabaseServer::stop() {
	m_database.stop();
}

void DatabaseServer::request_get(Client &client, const std::string &key) {
	if (!m_database.has(key))
		return send_null(client);

	auto value = m_database.get(key);
	switch (value.index()) {
		case 0:
			return send_ok(client, prepare(std::get<0>(value)));
		case 1:
			return send_ok(client, prepare(std::get<1>(value)));
		case 2:
			return send_ok(client, prepare(std::get<2>(value)));
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::request_set(Client &client, const std::string &key, const std::string &value) {
	m_database.set(key, value);
	send_ok(client);
}

void DatabaseServer::request_set(Client &client, const std::string &key, const int64_t &value) {
	m_database.set(key, value);
	send_ok(client);
}

void DatabaseServer::request_set(Client &client, const std::string &key, const double &value) {
	m_database.set(key, value);
	send_ok(client);
}

void DatabaseServer::request_del(Client &client, const std::string &key) {
	if (m_database.del(key))
		send_ok(client);
	else
		send_error(client);
}

void DatabaseServer::request_reset(Client &client) {
	m_database.reset();
	logger().info("Reset database");
	send_ok(client);
}

} // namespace vanity
