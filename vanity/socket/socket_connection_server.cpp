//
// Created by kingsli on 9/7/23.
//

#include "socket_connection_server.h"


namespace vanity{

SocketConnectionServer::SocketConnectionServer(uint16_t port) {
	m_socket.listen(port);
}

void SocketConnectionServer::register_event(int epoll_fd) {
	m_socket.register_event(epoll_fd, *this);
}

void SocketConnectionServer::unregister_event(int epoll_fd) const {
	m_socket.unregister_event(epoll_fd);
}

void SocketConnectionServer::ready(SocketServer& server) {
	// assumed to never fail
	server.add_client(ConcreteClient{m_socket.accept()});
}

} // namespace vanity