//
// Created by kingsli on 9/14/23.
//

#include "database_server.h"


namespace vanity {

void DatabaseServer::instruction_get(const ClientSocket &socket, const std::string &key) {
	if (!m_database.has(key))
		return send(socket, server_constants::null);

	auto value = m_database.get(key);
	switch (value.index()) {
		case 0:
			return send(socket, InstructionServer::prepare(std::get<0>(value)));
		case 1:
			return send(socket, InstructionServer::prepare(std::get<1>(value)));
		case 2:
			return send(socket, InstructionServer::prepare(std::get<2>(value)));
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::instruction_set(const ClientSocket &socket, const std::string &key, const std::string &value) {
	m_database.set(key, value);
	send(socket, server_constants::ok);
}

void DatabaseServer::instruction_set(const ClientSocket &socket, const std::string &key, const int64_t &value) {
	m_database.set(key, value);
	send(socket, server_constants::ok);
}

void DatabaseServer::instruction_set(const ClientSocket &socket, const std::string &key, const double &value) {
	m_database.set(key, value);
	send(socket, server_constants::ok);
}

void DatabaseServer::instruction_del(const ClientSocket &socket, const std::string &key) {
	if (m_database.del(key))
		send(socket, server_constants::ok);
	else
		send(socket, server_constants::error);
}

void DatabaseServer::instruction_reset(const ClientSocket &socket) {
	m_database.reset();
	logger().info("Reset database");
	send(socket, server_constants::ok);
}

} // namespace vanity
