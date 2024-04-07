#include <arpa/inet.h>
#include <unistd.h>

#include "exceptions.h"
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
	if (bytes >= 0)
		return bytes;

	if (errno == ECONNRESET)
		return 0;

	throw SocketError("Could not read from the socket");
}

size_t Socket::write(const char *buffer, size_t buffer_size) const {
	auto bytes = ::write(m_fd, buffer, buffer_size);
	if (bytes < 0)
		throw SocketError("Could not write to the socket");

	return bytes;
}

Socket Socket::connect(const char *host, uint16_t port) {
	auto fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd < 0)
		throw SocketError("Could not create the socket");

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, host, &addr.sin_addr);
	if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) and errno != EINPROGRESS)
		throw SocketError("Could not connect to the server");

	return Socket{fd};
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
