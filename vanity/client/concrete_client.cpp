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

struct session_info &ConcreteClient::session_info() {
	return m_session_info;
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

}

// namespace vanity