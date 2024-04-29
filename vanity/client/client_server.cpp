//
// Created by kingsli on 4/8/24.
//

#include "client_server.h"

namespace vanity {

/*
 * Behaves like an std::unique_ptr that
 * does not delete the pointer on destruction
*/
template<typename T>
class bad_ptr
{
private:
	std::unique_ptr<T> m_ptr;

public:
	// construct from a pointer
	explicit bad_ptr(T* ptr) : m_ptr{ptr} { }

	// convert to an std::unique_ptr&
	operator const std::unique_ptr<T>&() {
		return m_ptr;
	}

	// release the pointer without deleting it
	~bad_ptr() {
		std::ignore = m_ptr.release();
	}
};

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

TcpClient& ClientServer::add_client(TcpClient &&client) {
	auto& ptr = *m_clients.emplace(std::make_unique<TcpClient>(std::move(client))).first;
	epoll_add(*ptr);
	return *ptr;
}

void ClientServer::remove_client(TcpClient &client) {
	pre_client_delete(client);
	epoll_remove(client);
	m_clients.erase(bad_ptr{&client});
}

auto ClientServer::handle_callback(TcpClient& client) -> handle_callback_t {
	return [this, &client](auto msg) {
		handle(msg, client);
	};
}

void ClientServer::send(Client &client, Sendable&& sendable) {
	client.write(*this, sendable.move());
}

void ClientServer::stop() {
	m_clients.clear();
}

void ClientServer::pre_client_delete(TcpClient &client) {
	pre_client_delete_pubsub(client);
	pre_client_delete_peer(client);
}

} // namespace vanity
