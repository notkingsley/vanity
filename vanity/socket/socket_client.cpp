//
// Created by kingsli on 4/1/24.
//

#include "socket_client.h"

namespace vanity::socket {


SocketClient::SocketClient(Socket &&socket)
	: m_socket(std::move(socket)), m_writer(m_socket) {}

SocketClient::SocketClient(SocketClient &&other) noexcept
	: m_socket(std::move(other.m_socket)), m_writer(m_socket) {}

int SocketClient::socket_fd() const { return m_socket.fd(); }

size_t SocketClient::read(char *buffer, size_t buffer_size) const {
	return m_socket.read(buffer, buffer_size);
}

void SocketClient::write(WriteManager& manager, std::string &&response) {
	m_writer.write(manager, std::move(response));
}

} // namespace vanity::socket
