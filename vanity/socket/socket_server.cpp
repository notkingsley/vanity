//
// Created by kingsli on 9/7/23.
//

#include "socket_connection_server.h"
#include "socket_server.h"


namespace vanity {

SocketServer::SocketServer(std::vector<uint16_t> ports) : m_ports{std::move(ports)} {
	m_super_epoll.add(m_read_epoll);
	m_super_epoll.add(m_write_epoll);
}

void SocketServer::event_socket_ready() {
	socket_ready();
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
	m_connection_servers.emplace_back(port);
	m_read_epoll.add(m_connection_servers.back());
	logger().info("Listening on port " + std::to_string(port));
}

void SocketServer::bind_all() {
	for (auto& port : m_ports)
		bind(port);
}

void SocketServer::send(Client &client, Response&& response) {
	client.write(*this, response.move());
}

void SocketServer::poll() {
	constexpr int poll_size = 1;
	epoll_event events[poll_size] {};

	while (true) {
		int n = m_super_epoll.wait(events, poll_size, M_MAX_TIMEOUT * 1000);

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
			push_event(server_event::socket_ready);
			m_polled.wait();
		}

		if (not m_running)
			m_polled.clear();
	}
}

void SocketServer::start() {
	bind_all();
	m_running = true;
	m_poll_thread = std::thread{&SocketServer::poll, this};
}

void SocketServer::stop() {
	m_running = false;
	m_poll_thread.join();
	m_clients.clear();
	m_connection_servers.clear();
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

		for (int i = 0; i < n; ++i){
			auto handler = static_cast<SocketEventHandler *>(events[i].data.ptr);
			handler->ready(*this);
		}
	}
}

void SocketServer::add_client(ConcreteClient &&client) {
	auto it = m_clients.emplace(std::move(client)).first;
	m_read_epoll.add(const_cast<ConcreteClient&>((*it)));
}

void SocketServer::remove_client(ConcreteClient &client) {
	m_read_epoll.remove(client);
	m_clients.erase(client);
}

void SocketServer::add_socket_writer(SocketWriter &writer) {
	m_write_epoll.add(writer);
}

void SocketServer::remove_socket_writer(SocketWriter &writer) {
	m_write_epoll.remove(writer);
}

} // namespace vanity