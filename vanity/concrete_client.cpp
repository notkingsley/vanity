//
// Created by kingsli on 11/15/23.
//

#include "concrete_client.h"
#include "socket/socket_server.h"

namespace vanity {

ConcreteClient::ConcreteClient(ClientSocket&& socket) : SocketClient(std::move(socket)) {}

void ConcreteClient::ready(SocketServer &server) {
	try{
		if(m_closed or not m_reader.read(server, *this))
			// Warning: this will delete this object
			server.remove_client(*this);
	}
	catch (DestroyClient& e) {
		// Warning: this will delete this object
		server.remove_client(*this);
	}
}

bool ConcreteClient::has_perm(operation_t op) const {
	return is_permitted(op, m_session_info.auth);
}

void ConcreteClient::db(int db) {
	m_session_info.database = db;
}

int ConcreteClient::db() const {
	return m_session_info.database;
}

void ConcreteClient::set_auth(const client_auth &auth) {
	m_session_info.auth = auth;
}

void ConcreteClient::username(const std::string &username) {
	m_session_info.username = username;
}

const std::string &ConcreteClient::username() const {
	return m_session_info.username;
}

void ConcreteClient::close() {
	m_closed = true;
}

size_t ConcreteClient::read(char *buffer, size_t buffer_size) const {
	return SocketClient::read(buffer, buffer_size);
}

void ConcreteClient::write(SocketServer &server, Response &&response) {
	SocketClient::write(server, std::move(response));
}

bool operator==(const ConcreteClient& lhs, const ConcreteClient& rhs) {
	return &lhs == &rhs;
}

} // namespace vanity