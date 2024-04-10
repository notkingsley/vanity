//
// Created by kingsli on 4/8/24.
//

#include "client_server.h"

namespace vanity {

void ClientServer::read_handler_ready(SocketReadHandler *handler) {
	if (auto client = dynamic_cast<socket::ClientReadHandler*>(handler))
		client->ready(as_client_manager());
	else
		handler->ready(as_read_manager());
}

ClientManager &ClientServer::as_client_manager() {
	return *this;
}

ReadManager &ClientServer::as_read_manager() {
	return *this;
}

void ClientServer::add_client(TcpClient &&client) {
	auto& ptr = *m_clients.emplace(std::make_unique<TcpClient>(std::move(client))).first;
	epoll_add(*ptr);
}

void ClientServer::remove_client(TcpClient &client) {
	epoll_remove(client);
	std::unique_ptr<TcpClient> ptr{&client};
	m_clients.erase(ptr);
	ptr.release();
}

auto ClientServer::handle_callback(TcpClient& client) -> handle_callback_t {
	return [this, &client](auto msg) {
		handle(msg, client);
	};
}

void ClientServer::send(Client &client, Response&& response) {
	client.write(*this, response.move());
}

void ClientServer::stop() {
	m_clients.clear();
}

} // namespace vanity
