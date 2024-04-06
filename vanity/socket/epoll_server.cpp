//
// Created by kingsli on 4/4/24.
//

#include "epoll_server.h"
#include "exceptions.h"
#include "socket_writer.h"


namespace vanity::socket {

EpollServer::EpollServer() {
	m_super_epoll.add(m_read_epoll);
	m_super_epoll.add(m_write_epoll);
}

void EpollServer::add_read_handler(SocketReadHandler &read_handler) {
	epoll_add(read_handler);
}

void EpollServer::remove_read_handler(SocketReadHandler &read_handler) {
	epoll_remove(read_handler);
}

void EpollServer::add_writer(SocketWriter &writer) {
	m_write_epoll.add(writer);
}

void EpollServer::remove_writer(SocketWriter &writer) {
	m_write_epoll.remove(writer);
}

void EpollServer::epoll_add(SocketReadHandler &handler) {
	m_read_epoll.add(handler);
}

void EpollServer::epoll_remove(SocketReadHandler &handler) {
	m_read_epoll.remove(handler);
}

int EpollServer::epoll_wait(int timeout) {
	static constexpr int poll_size = 1;
	static epoll_event events[poll_size] {};

	int n = m_super_epoll.wait(events, poll_size, timeout);

	if (n < 0) {
		SocketError err{{}};
		if (not err.is_interrupt())
			logger().error("Could not wait for epoll: " + std::to_string(err.get_errno()));
	}

	return n;
}

void EpollServer::epoll_ready() {
	static constexpr int super_poll_size = 2;
	static epoll_event events[super_poll_size] {};

	int n = m_super_epoll.wait(events, super_poll_size, 0);

	if (n < 0) {
		SocketError err{{}};
		logger().error("Could not wait for epoll: " + std::to_string(err.get_errno()));
	}

	for (int i = 0; i < n; ++i) {
		auto epoll = static_cast<Epoll *>(events[i].data.ptr);
		epoll_ready(*epoll);
	}
}

void EpollServer::epoll_ready(Epoll &epoll) {
	static constexpr int poll_size = 10;
	static epoll_event events[poll_size] {};

	while (true) {
		int n = epoll.wait(events, poll_size, 0);

		if (n < 0) {
			SocketError err{{}};
			logger().error("Could not wait for epoll: " + std::to_string(err.get_errno()));
			break;
		}

		if (n == 0)
			break;

		for (int i = 0; i < n; ++i)
			if (&epoll == &m_read_epoll)
				read_ready(events[i]);
			else if (&epoll == &m_write_epoll)
				write_ready(events[i]);
			else
				logger().error("Unknown epoll");
	}
}

void EpollServer::read_ready(epoll_event &event) {
	auto handler = static_cast<SocketReadHandler*>(event.data.ptr);
	read_handler_ready(handler);
}

void EpollServer::write_ready(epoll_event &event) {
	auto handler = static_cast<SocketWriteHandler*>(event.data.ptr);
	handler->ready(*this);
}

} // namespace vanity::socket
