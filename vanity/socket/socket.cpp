#include <unistd.h>

#include "socket.h"


namespace vanity::socket {

Socket::~Socket()
{
	if (m_fd >= 0)
		close(m_fd);
}

Socket::Socket(Socket &&other) noexcept {
	m_fd = other.m_fd;
	other.m_fd = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
	m_fd = other.m_fd;
	other.m_fd = -1;
	return *this;
}

int Socket::fd() const {
	return m_fd;
}

ClientSocket::ClientSocket(int server_fd)
{
	sockaddr_in m_addr{};
	socklen_t m_addr_size = sizeof(m_addr);

	m_fd = accept4(server_fd, (sockaddr*)&m_addr, &m_addr_size, SOCK_NONBLOCK);
	if (m_fd < 0)
		throw SocketError("Could not accept the connection");
}

size_t ClientSocket::read(char* buffer, size_t buffer_size) const
{
	auto bytes_read = ::read(m_fd, buffer, buffer_size);
	if (bytes_read < 0)
		throw SocketError("Could not read from the socket");

	return bytes_read;
}

size_t ClientSocket::write(const char *buffer, size_t buffer_size) const {
	auto bytes_written = ::write(m_fd, buffer, buffer_size);
	if (bytes_written < 0)
		throw SocketError("Could not write to the socket");

	return bytes_written;
}


ServerSocket::ServerSocket()
{
	m_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (m_fd < 0)
		throw SocketError("Could not create the server socket");
}

void ServerSocket::listen(int port)
{
	sockaddr_in m_addr{};
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons(port);

	if (bind(m_fd, (sockaddr*)&m_addr, sizeof(m_addr)) < 0)
		throw SocketError("Could not bind the socket");

	if (::listen(m_fd, SOMAXCONN) < 0)
		throw SocketError("Could not listen on the socket");
}

ClientSocket ServerSocket::accept()
{
	return ClientSocket{m_fd};
}

} // namespace vanity::socket
