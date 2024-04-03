//
// Created by kingsli on 11/15/23.
//

#include "exceptions.h"
#include "tcp_client.h"


namespace vanity {

TcpClient::TcpClient(Socket&& socket) : m_socket(std::move(socket)), m_writer(m_socket) {}

void TcpClient::ready(ClientManager& manager) {
	try{
		auto callback = manager.handle_callback(*this);
		if (not m_reader.read(m_socket, callback))
			// Warning: this will delete this object
			manager.remove_client(*this);
	}
	catch (DestroyClient& e) {
		// Warning: this will delete this object
		manager.remove_client(*this);
	}
}

bool TcpClient::has_perm(operation_t op) const {
	return is_permitted(op, m_session_info.auth);
}

struct session_info &TcpClient::session_info() {
	return m_session_info;
}

void TcpClient::write(WriteManager& manager, Response &&response) {
	m_writer.write(manager, response.extract_data());
}

int TcpClient::socket_fd() const {
	return m_socket.fd();
}

TcpClient::TcpClient(TcpClient &&other) noexcept
	: m_socket(std::move(other.m_socket)), m_writer(m_socket) {}


bool operator==(const TcpClient& lhs, const TcpClient& rhs) {
	return &lhs == &rhs;
}

} // namespace vanity
