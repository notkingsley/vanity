//
// Created by kingsli on 9/7/23.
//

#include "client/concrete_client.h"
#include "socket_listener.h"


namespace vanity::socket {

SocketListener::SocketListener(uint16_t port) {
	m_socket.listen(port);
}

int SocketListener::socket_fd() const {
	return m_socket.fd();
}

void SocketListener::ready(ClientManager& manager) {
	// assumed to never fail
	manager.add_client(ConcreteClient{m_socket.accept()});
}

} // namespace vanity::socket
