#include <unistd.h>

#include "socket.h"


namespace vanity::socket {

BaseSocket::~BaseSocket() {
	if (m_fd >= 0)
		close(m_fd);
}

BaseSocket::BaseSocket(BaseSocket &&other) noexcept {
	m_fd = other.m_fd;
	other.m_fd = -1;
}

BaseSocket &BaseSocket::operator=(BaseSocket &&other) noexcept {
	m_fd = other.m_fd;
	other.m_fd = -1;
	return *this;
}

int BaseSocket::fd() const {
	return m_fd;
}

Socket::Socket(int fd) {
	m_fd = fd;
}

size_t Socket::read(char* buffer, size_t buffer_size) const
{
	auto bytes = ::read(m_fd, buffer, buffer_size);
	if (bytes < 0)
		throw SocketError("Could not read from the socket");

	return bytes;
}

size_t Socket::write(const char *buffer, size_t buffer_size) const {
	auto bytes = ::write(m_fd, buffer, buffer_size);
	if (bytes < 0)
		throw SocketError("Could not write to the socket");

	return bytes;
}


ServerSocket::ServerSocket() {
	m_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (m_fd < 0)
		throw SocketError("Could not create the server socket");
}

void ServerSocket::listen(int port) {
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(m_fd, (sockaddr*)&addr, sizeof(addr)) < 0)
		throw SocketError("Could not bind the socket");

	if (::listen(m_fd, SOMAXCONN) < 0)
		throw SocketError("Could not listen on the socket");
}

Socket ServerSocket::accept() {
	sockaddr_in addr{};
	socklen_t addr_size = sizeof(addr);

	auto fd = accept4(m_fd, (sockaddr*)&addr, &addr_size, SOCK_NONBLOCK);
	if (m_fd < 0)
		throw SocketError("Could not accept the connection");

	return Socket{fd};
}

} // namespace vanity::socket
