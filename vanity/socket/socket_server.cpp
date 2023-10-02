//
// Created by kingsli on 9/7/23.
//

#include "socket_connection_server.h"
#include "socket_server.h"
#include "socket_writer.h"


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

void SocketServer::socket_ready() {
	constexpr int poll_size = 10;
	epoll_event events[poll_size] {};

	while (true) {
		int n = epoll_wait(m_epoll_fd, events, poll_size, 0);

		if (n < 0){
			SocketError err{{}};
			logger().error("Could not wait for epoll: " + std::to_string(err.get_errno()));
			break;
		}

		if (n == 0)
			break;

		for (int i = 0; i < n; ++i) {
			auto handler = static_cast<SocketEventHandler *>(events[i].data.ptr);
			try{
				if (!handler->ready(*this)){
					remove_socket_handler(*handler);
					logger().debug("Client Socket closed");
				}
			}
			catch (DestroyClient& e)
			{
				remove_socket_handler(*handler);
				logger().debug("Client destroyed.");
			}
		}
	}
	m_polled.set();
}

void SocketServer::bind(uint16_t port) {
	add_socket_handler(std::make_unique<SocketConnectionServer>(*this, port));
	logger().info("Listening on port " + std::to_string(port));
}

void SocketServer::send(const ClientSocket &client, const std::string& msg) {
	// TODO: actually register SocketWriters for write events in a separate epoll
	// add_socket_handler(std::make_unique<SocketWriter>(client, std::move(msg)));

	SocketWriter writer{client, msg};
	while (writer.ready(*this));
}

void SocketServer::poll() {
	constexpr int poll_size = 1;
	epoll_event events[poll_size] {};

	while (true) {
		int n = epoll_wait(m_epoll_fd, events, poll_size, M_MAX_TIMEOUT);

		if (n < 0){
			SocketError err{{}};
			if (err.get_errno() == EINTR)
				continue;
			else
				logger().error("Could not wait for epoll: " + std::to_string(err.get_errno()));
		}

		if (n == 0 and not m_running)
			break;

		if (n > 0){
			m_event_queue.push(server_event::socket_ready);
			m_polled.wait();
		}

		if (not m_running)
			m_polled.clear();
	}
}

void SocketServer::start() {
	m_running = true;
	m_poll_thread = std::thread{&SocketServer::poll, this};
}

void SocketServer::stop() {
	m_running = false;
	m_poll_thread.join();
	m_handlers.clear();
}

} // namespace vanity