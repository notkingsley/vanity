//
// Created by kingsli on 9/7/23.
//

#include "socket_listener.h"
#include "socket_server.h"

namespace vanity::socket {

SocketListener::SocketListener(uint16_t port) {
	m_socket.listen(port);
}

int SocketListener::socket_fd() const {
	return m_socket.fd();
}

void SocketListener::ready(SocketServer& server) {
	// assumed to never fail
	server.add_client(ConcreteClient{m_socket.accept()});
}

} // namespace vanity::socket
