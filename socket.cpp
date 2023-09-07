#include <unistd.h>

#include "socket.h"


namespace vanity{

Socket::~Socket()
{
	if (m_fd >= 0)
		close(m_fd);
}

Socket::Socket(Socket &&other) noexcept {
	m_fd = other.m_fd;
	m_addr = other.m_addr;
	m_addr_size = other.m_addr_size;
	other.m_fd = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
	m_fd = other.m_fd;
	m_addr = other.m_addr;
	m_addr_size = other.m_addr_size;
	other.m_fd = -1;
	return *this;
}

void Socket::register_event(int epoll_fd, SocketEventHandler& handler) const {
	handler.register_event(epoll_fd, m_fd);
}

void Socket::unregister_event(int epoll_fd, SocketEventHandler &handler) const {
	SocketEventHandler::unregister_event(epoll_fd, m_fd);
}


ClientSocket::ClientSocket(int server_fd)
{
	m_fd = accept(server_fd, (sockaddr*)&m_addr, &m_addr_size);
	if (m_fd < 0){
		throw SocketError("Could not accept the connection");
	}
}

size_t ClientSocket::read(char* buffer, size_t buffer_size)
{
	auto bytes_read = ::read(m_fd, buffer, buffer_size);
	if (bytes_read < 0){
		throw SocketError("Could not read from the socket");
	}
	return bytes_read;
}

void ClientSocket::write(const std::string& msg)
{
	if (::write(m_fd, msg.c_str(), msg.length()) < 0)
		throw SocketError("Could not write to the socket");
}


ServerSocket::ServerSocket()
{
	m_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_fd < 0){
		throw SocketError("Could not create the server socket");
	}
}

void ServerSocket::listen(int port)
{
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons(port);

	if (bind(m_fd, (sockaddr*)&m_addr, sizeof(m_addr)) < 0){
		throw SocketError("Could not bind the socket");
	}
	if (::listen(m_fd, SOMAXCONN) < 0){
		throw SocketError("Could not listen on the socket");
	}
}

ClientSocket ServerSocket::accept()
{
	return ClientSocket{m_fd};
}

} // namespace vanity