#include <arpa/inet.h>
#include <stdexcept>
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
	struct addrinfo hints{};
	hints.ai_family = AF_INET;    // Allow IPv4
	hints.ai_socktype = SOCK_STREAM; // Stream socket
	hints.ai_flags = AI_PASSIVE;    // For wildcard IP address
	hints.ai_protocol = 0;          // Any protocol

	struct addrinfo *result;
	auto s = getaddrinfo(host, std::to_string(port).c_str(), &hints, &result);
	if (s != 0)
		throw SocketError("getaddrinfo: " + std::string(gai_strerror(s)));

	int fd;
	struct addrinfo *rp;
	for (rp = result; rp != nullptr; rp = rp->ai_next) {
		fd = ::socket(rp->ai_family, rp->ai_socktype | SOCK_NONBLOCK, rp->ai_protocol);
		if (fd == -1)
			continue;

		if (::connect(fd, rp->ai_addr, rp->ai_addrlen) != -1 || errno == EINPROGRESS)
			break;                  // Success

		close(fd);
	}

	freeaddrinfo(result);           // No longer needed

	if (rp == nullptr)               // No address succeeded
		throw SocketError("Could not connect to the server");

	return Socket{fd};
}

std::pair<std::string, uint16_t> Socket::get_remote_addr() const {
	sockaddr_in addr{};
	socklen_t addr_size = sizeof(addr);

	if (getpeername(m_fd, (sockaddr*)&addr, &addr_size) < 0)
		throw SocketError("Could not get the remote socket name");

	char host[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
	return {host, ntohs(addr.sin_port)};
}

std::pair<std::string, uint16_t> Socket::get_local_addr() const {
	sockaddr_in addr{};
	socklen_t addr_size = sizeof(addr);

	if (getsockname(m_fd, (sockaddr*)&addr, &addr_size) < 0)
		throw SocketError("Could not get the local socket name");

	char host[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
	return {host, ntohs(addr.sin_port)};
}


ServerSocket::ServerSocket() {
	m_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (m_fd < 0)
		throw SocketError("Could not create the server socket");
}

void ServerSocket::listen(const char* host, uint16_t port) {
	struct addrinfo hints{};
	hints.ai_family = AF_INET;    // Allow IPv4
	hints.ai_socktype = SOCK_STREAM; // Stream socket
	hints.ai_flags = AI_PASSIVE;    // For wildcard IP address
	hints.ai_protocol = 0;          // Any protocol

	struct addrinfo *result;
	auto s = getaddrinfo(host, std::to_string(port).c_str(), &hints, &result);
	if (s != 0)
		throw std::runtime_error("getaddrinfo: " + std::string(gai_strerror(s)));

	struct addrinfo *rp;
	for (rp = result; rp != nullptr; rp = rp->ai_next)
		if (bind(m_fd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;                  // Success

	freeaddrinfo(result);           // No longer needed

	if (rp == nullptr)               // No address succeeded
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

std::pair<std::string, uint16_t> ServerSocket::get_host_and_port() const {
	sockaddr_in addr{};
	socklen_t addr_size = sizeof(addr);

	if (getsockname(m_fd, (sockaddr*)&addr, &addr_size) < 0)
		throw SocketError("Could not get the socket name");

	char host[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
	return {host, ntohs(addr.sin_port)};
}

} // namespace vanity::socket
