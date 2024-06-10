//
// Created by kingsli on 12/24/23.
//

#include "event_server.h"

namespace vanity {

void EventServer::event_loop()  {
	while (true)
		switch (m_event_queue.get()) {
			case server_event::epoll_ready: {
				event_epoll_ready();
				break;
			}
			case server_event::persist: {
				event_persist();
				break;
			}
			case server_event::expire: {
				event_expire();
				break;
			}
			case server_event::publish: {
				event_publish();
				break;
			}
			case server_event::pulse: {
				event_pulse();
				break;
			}
			case server_event::terminate: {
				push_event(server_event::terminate);
				return;
			}
		}
}

void EventServer::push_event(EventServer::server_event event) {
	m_event_queue.push(event);
}

void EventServer::terminate() {
	push_event(server_event::terminate);
}

void EventServer::run() {
	std::array<std::thread, M_THREADS - 1> threads;
	for (auto& thread : threads)
		thread = std::thread{[this] { event_loop(); }};

	event_loop();
	for (auto& thread : threads)
		thread.join();

	m_event_queue.clear();
}

} // namespace vanity