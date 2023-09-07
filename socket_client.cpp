//
// Created by kingsli on 9/6/23.
//

#include "socket_client.h"


namespace vanity{

SocketClient::SocketClient(ClientSocket&& socket)
	: m_socket{std::move(socket)}, m_reader{m_socket} {}

bool SocketClient::ready(AbstractBaseServer& server)
{
	auto res = m_reader.read(server);
	if (not res.first){
		std::cout << "Connection closed\n";
	}

	return res.first;
}

void SocketClient::register_event(int epoll_fd) {
	m_socket.register_event(epoll_fd, *this);
}

void SocketClient::unregister_event(int epoll_fd) {
	m_socket.unregister_event(epoll_fd);
}

}