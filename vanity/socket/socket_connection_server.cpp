//
// Created by kingsli on 9/7/23.
//

#include "socket_connection_server.h"
#include "socket_server.h"

namespace vanity{

SocketConnectionServer::SocketConnectionServer(uint16_t port) {
	m_socket.listen(port);
}

int SocketConnectionServer::socket_fd() const {
	return m_socket.fd();
}

void SocketConnectionServer::ready(SocketServer& server) {
	// assumed to never fail
	server.add_client(ConcreteClient{m_socket.accept()});
}

} // namespace vanity