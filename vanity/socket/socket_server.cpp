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
	m_clients.clear();
	m_listeners.clear();
}

void SocketServer::add_client(TcpClient &&client) {
	auto& c = *m_clients.emplace(std::move(client)).first;
	epoll_add(const_cast<TcpClient&>(c));
}

void SocketServer::remove_client(TcpClient &client) {
	epoll_remove(client);
	m_clients.erase(client);
}

auto SocketServer::handle_callback(TcpClient& client) -> handle_callback_t {
	return [this, &client](auto msg) {
		handle(msg, client);
	};
}

void SocketServer::bind_all() {
	m_listeners.reserve(m_ports.size());

	for (auto& port : m_ports) {
		m_listeners.emplace_back(port);
		epoll_add(m_listeners.back());
		logger().info("Listening on port " + std::to_string(port));
	}
}

void SocketServer::send(Client &client, Response&& response) {
	client.write(*this, response.move());
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

void SocketServer::read_handler_ready(SocketReadHandler *handler) {
	if (auto client = dynamic_cast<ClientReadHandler*>(handler))
		client->ready(as_client_manager());
	else
		handler->ready(as_read_manager());
}

ClientManager &SocketServer::as_client_manager() {
	return *this;
}

ReadManager &SocketServer::as_read_manager() {
	return *this;
}

} // namespace vanity::socket
