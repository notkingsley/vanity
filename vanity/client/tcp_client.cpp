//
// Created by kingsli on 11/15/23.
//

#include "exceptions.h"
#include "tcp_client.h"


namespace vanity {

TcpClient::TcpClient(Socket&& socket) : m_socket(std::move(socket)), m_writer(m_socket) {}

TcpClient::TcpClient(TcpClient &&other) noexcept
	: m_socket(std::move(other.m_socket)), m_writer(m_socket) {}

void TcpClient::ready(ClientManager& manager) {
	auto callback = manager.handle_callback(*this);
	if (not m_closed)
		if (not m_reader.read(m_socket, callback))
			m_closed = true;

	if (m_closed)
		manager.remove_client(*this);
}

bool TcpClient::has_perm(operation_t op) const {
	return is_permitted(op, m_session_info.auth);
}

struct session_info &TcpClient::session_info() {
	return m_session_info;
}

void TcpClient::write(WriteManager& manager, Sendable &&sendable) {
	m_writer.write(manager, sendable.extract_data());
}

int TcpClient::socket_fd() const {
	return m_socket.fd();
}

void TcpClient::close() {
	m_closed = true;
}

} // namespace vanity
