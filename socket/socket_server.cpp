//
// Created by kingsli on 9/7/23.
//

#include "socket_connection_server.h"
#include "socket_server.h"


namespace vanity {

SocketServer::SocketServer() {
	m_epoll_fd = epoll_create1(0);
	if (m_epoll_fd < 0)
		throw SocketError("Could not create epoll");
}

SocketServer::SocketServer(SocketServer &&other) noexcept {
	m_epoll_fd = other.m_epoll_fd;
	other.m_epoll_fd = -1;
}

SocketServer &SocketServer::operator=(SocketServer &&other) noexcept {
	m_epoll_fd = other.m_epoll_fd;
	other.m_epoll_fd = -1;
	return *this;
}

SocketServer::~SocketServer() {
	if (m_epoll_fd >= 0)
		close(m_epoll_fd);
}

void SocketServer::add_socket_handler(std::unique_ptr<SocketEventHandler>&& handler) {
	handler->register_event(m_epoll_fd);
	m_handlers.insert(std::move(handler));
}

void SocketServer::remove_socket_handler(SocketEventHandler &handler) {
	handler.unregister_event(m_epoll_fd);
	auto ptr = static_cast<const std::unique_ptr<SocketEventHandler>>(&handler);
	m_handlers.erase(ptr);
	// object is already destroyed here
	ptr.release();
}

void SocketServer::start() {
	while (true) {
		int n = epoll_wait(m_epoll_fd, m_events, MAX_EVENTS, EPOLL_TIMEOUT);
		if (n < 0)
			throw SocketError("Could not wait for epoll");

		for (int i = 0; i < n; ++i) {
			auto *ready_handler = static_cast<SocketEventHandler *>(m_events[i].data.ptr);
			if (!ready_handler->ready(*this)){
				remove_socket_handler(*ready_handler);
				return;
			}
		}
	}
}

void SocketServer::listen(uint16_t port) {
	add_socket_handler(std::make_unique<SocketConnectionServer>(*this, port));
}

} // namespace vanity