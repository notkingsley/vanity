//
// Created by kingsli on 9/7/23.
//

#ifndef VANITY_SOCKET_CONNECTION_SERVER_H
#define VANITY_SOCKET_CONNECTION_SERVER_H


#include "socket.h"
#include "socket_event_handler.h"
#include "socket_server.h"


namespace vanity{

/*
 * A SocketConnectionServer accepts connections with a socket server and yields SocketClients
 */
class SocketConnectionServer : public SocketReadHandler
{
private:
	ServerSocket m_socket;
	SocketServer& m_server;

public:
	// start to listen on port
	explicit SocketConnectionServer(SocketServer& server, uint16_t port);

	// register to epoll for events
	void register_event(int epoll_fd) override;

	// unregister from epoll for events
	void unregister_event(int epoll_fd) override;

	// the socket is ready for reading
	// return true if the socket should be kept, false otherwise
	bool ready() override;
};

} // namespace vanity

#endif //VANITY_SOCKET_CONNECTION_SERVER_H
