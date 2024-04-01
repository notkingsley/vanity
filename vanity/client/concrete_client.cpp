//
// Created by kingsli on 11/15/23.
//

#include "concrete_client.h"
#include "socket/socket_server.h"

namespace vanity {

ConcreteClient::ConcreteClient(Socket&& socket) : m_socket(std::move(socket)), m_writer(m_socket) {}

void ConcreteClient::ready(SocketServer &server) {
	try{
		auto callback = [this, &server](const std::string &data) {
			server.handle(data, *this);
		};
		if (not m_reader.read(m_socket, callback))
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

void ConcreteClient::write(WriteManager& manager, Response &&response) {
	m_writer.write(manager, response.extract_data());
}

int ConcreteClient::socket_fd() const {
	return m_socket.fd();
}

ConcreteClient::ConcreteClient(ConcreteClient &&other) noexcept
	: m_socket(std::move(other.m_socket)), m_writer(m_socket) {}


bool operator==(const ConcreteClient& lhs, const ConcreteClient& rhs) {
	return &lhs == &rhs;
}

} // namespace vanity
