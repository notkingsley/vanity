//
// Created by kingsli on 9/7/23.
//

#include "socket_server.h"

namespace vanity::socket {

void SocketServer::start() {
	BindServer::start();
	m_running = true;
	m_poll_thread = std::thread{&SocketServer::poll, this};
}

void SocketServer::stop() {
	m_running = false;
	m_poll_thread.join();
	ClientServer::stop();
	BindServer::stop();
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
