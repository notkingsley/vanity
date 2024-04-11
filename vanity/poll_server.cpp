//
// Created by kingsli on 4/11/24.
//

#include "poll_server.h"

namespace vanity {

void PollServer::event_socket_ready() {
	epoll_ready();
	m_polled.set();
}

void PollServer::start() {
	m_running = true;
	m_poll_thread = std::thread{&PollServer::poll, this};
}

void PollServer::stop() {
	m_running = false;
	m_poll_thread.join();
}

void PollServer::poll() {
	while (true) {
		auto n = epoll_wait(M_MAX_TIMEOUT * 1000);

		if (n == 0 and not m_running)
			break;

		if (n <= 0)
			continue;

		push_event(server_event::socket_ready);
		m_polled.wait();
		m_polled.clear();
	}
}

} // namespace vanity
