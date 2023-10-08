//
// Created by kingsli on 9/7/23.
//

#include "socket_connection_server.h"
#include "socket_server.h"
#include "socket_writer.h"


namespace vanity {

SocketServer::SocketServer() {
	m_super_epoll.add(m_read_epoll);
	m_super_epoll.add(m_write_epoll);
}

void SocketServer::add_socket_handler(std::unique_ptr<SocketEventHandler>&& handler) {
	if (dynamic_cast<SocketReadHandler*>(handler.get()))
		m_read_epoll.add(*handler);
	else if (dynamic_cast<SocketWriteHandler*>(handler.get()))
		m_write_epoll.add(*handler);
	else
		return logger().error(
			"SocketEventHandler must be either SocketReadHandler or SocketWriteHandler"
		);

	m_handlers.insert(std::move(handler));
}

void SocketServer::remove_socket_handler(SocketEventHandler &handler) {
	if (dynamic_cast<SocketReadHandler*>(&handler))
		m_read_epoll.remove(handler);
	else if (dynamic_cast<SocketWriteHandler*>(&handler))
		m_write_epoll.remove(handler);
	else
		return logger().error(
			"SocketEventHandler must be either SocketReadHandler or SocketWriteHandler"
		);

	auto ptr = static_cast<const std::unique_ptr<SocketEventHandler>>(&handler);
	m_handlers.erase(ptr); // object is destroyed here
	ptr.release();
}

void SocketServer::socket_ready() {
	constexpr int super_poll_size = 2;
	epoll_event events[super_poll_size] {};

	int n = m_super_epoll.wait(events, super_poll_size, 0);

	if (n < 0) {
		SocketError err{{}};
		logger().error("Could not wait for epoll: " + std::to_string(err.get_errno()));
	}

	for (int i = 0; i < n; ++i) {
		auto epoll = static_cast<Epoll *>(events[i].data.ptr);
		epoll_ready(*epoll);
	}

	m_polled.set();
}

void SocketServer::bind(uint16_t port) {
	add_socket_handler(std::make_unique<SocketConnectionServer>(*this, port));
	logger().info("Listening on port " + std::to_string(port));
}

void SocketServer::send(const Client &client, const std::string& msg) {
	 add_socket_handler(std::make_unique<SocketWriter>(client.socket(), msg));
}

void SocketServer::poll() {
	constexpr int poll_size = 1;
	epoll_event events[poll_size] {};

	while (true) {
		int n = m_super_epoll.wait(events, poll_size, M_MAX_TIMEOUT);

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

void SocketServer::epoll_ready(Epoll &epoll) {
	constexpr int poll_size = 10;
	epoll_event events[poll_size] {};

	while (true) {
		int n = epoll.wait(events, poll_size, 0);

		if (n < 0){
			SocketError err{{}};
			logger().error("Could not wait for epoll: " + std::to_string(err.get_errno()));
			break;
		}

		if (n == 0)
			break;

		for (int i = 0; i < n; ++i) {
			auto handler = static_cast<SocketEventHandler *>(events[i].data.ptr);
			handler_ready(handler);
		}
	}

}

void SocketServer::handler_ready(SocketEventHandler* handler) {
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

} // namespace vanity