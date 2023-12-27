//
// Created by kingsli on 9/7/23.
//

#ifndef VANITY_SOCKET_CONNECTION_SERVER_H
#define VANITY_SOCKET_CONNECTION_SERVER_H


#include "socket.h"
#include "socket_event_handler.h"

namespace vanity{

/*
 * A SocketConnectionServer accepts connections with a
 * server socket and yields Clients to the server
 */
class SocketConnectionServer : public SocketReadHandler
{
private:
	ServerSocket m_socket;

public:
	// start to listen on port
	explicit SocketConnectionServer(uint16_t port);

	// get the socket file descriptor
	int socket_fd() const override;

	// the socket is ready for reading
	// return true if the socket should be kept, false otherwise
	void ready(SocketServer& server) override;
};

} // namespace vanity

#endif //VANITY_SOCKET_CONNECTION_SERVER_H
