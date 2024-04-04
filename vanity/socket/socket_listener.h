//
// Created by kingsli on 9/7/23.
//

#ifndef VANITY_SOCKET_LISTENER_H
#define VANITY_SOCKET_LISTENER_H

#include "socket.h"
#include "client_read_handler.h"


namespace vanity::socket {

/*
 * A SocketListener accepts connections with a
 * server socket and yields Clients to the server
 */
class SocketListener : public ClientReadHandler
{
private:
	ServerSocket m_socket;

public:
	// start to listen on port
	explicit SocketListener(uint16_t port);

	// get the socket file descriptor
	int socket_fd() const override;

	// the socket is ready for reading
	// return true if the socket should be kept, false otherwise
	void ready(ClientManager& manager) override;
};

} // namespace vanity::socket

#endif //VANITY_SOCKET_LISTENER_H
