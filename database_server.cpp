//
// Created by kingsli on 9/14/23.
//

#include "database_server.h"


namespace vanity {

void DatabaseServer::instruction_get(const ClientSocket &socket, const std::string &key) {
	if (m_database.has(key))
		send(socket, InstructionServer::make_message(m_database.get(key)));
	else
		send(socket, server_constants::null);
}

void DatabaseServer::instruction_set(const ClientSocket &socket, const std::string &key, const std::string &value) {
	m_database.set(key, value);
	send(socket, server_constants::ok);
}

void DatabaseServer::instruction_del(const ClientSocket &socket, const std::string &key) {
	if (m_database.del(key))
		send(socket, server_constants::ok);
	else
		send(socket, server_constants::error);
}

} // namespace vanity
