//
// Created by kingsli on 9/7/23.
//

#include "socket_server.h"

namespace vanity::socket {

SocketServer::SocketServer(std::vector<uint16_t> ports) : m_ports{std::move(ports)} { }

void SocketServer::start() {
	bind_all();
	m_running = true;
	m_poll_thread = std::thread{&SocketServer::poll, this};
}

void SocketServer::stop() {
	m_running = false;
	m_poll_thread.join();
	ClientServer::stop();
	m_listeners.clear();
}

void SocketServer::bind_all() {
	m_listeners.reserve(m_ports.size());

	for (auto& port : m_ports) {
		m_listeners.emplace_back(port);
		epoll_add(m_listeners.back());
		logger().info("Listening on port " + std::to_string(port));
	}
}

void SocketServer::event_socket_ready() {
	epoll_ready();
	m_polled.set();
}

void SocketServer::poll() {
	while (true) {
		auto n = epoll_wait(M_MAX_TIMEOUT * 1000);

		if (n == 0 and not m_running)
			break;

		if (n > 0) {
			push_event(server_event::socket_ready);
			m_polled.wait();
			m_polled.clear();
		}
	}
}

} // namespace vanity::socket
