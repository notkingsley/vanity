//
// Created by kingsli on 9/7/23.
//

#include "socket_client.h"
#include "socket_connection_server.h"


namespace vanity{

SocketConnectionServer::SocketConnectionServer(SocketServer& server, uint16_t port) : m_server{server} {
	m_socket.listen(port);
}

void SocketConnectionServer::register_event(int epoll_fd) {
	m_socket.register_event(epoll_fd, *this);
}

void SocketConnectionServer::unregister_event(int epoll_fd) {
	m_socket.unregister_event(epoll_fd);
}

bool SocketConnectionServer::ready(AbstractBaseServer& server) {
	m_server.add_handler(std::make_unique<SocketClient>(m_socket.accept()));
	return true; 	// TODO: handle errors if server socket is closed
}

} // namespace vanity