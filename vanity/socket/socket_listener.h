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
	explicit SocketListener(const char* host, uint16_t port);

	// no copy
	SocketListener(const SocketListener&) = delete;
	SocketListener& operator=(const SocketListener&) = delete;

	// no move
	SocketListener(SocketListener&&) = delete;
	SocketListener& operator=(SocketListener&&) = delete;

	// return the host and port of the server
	std::pair<std::string, uint16_t> get_host_and_port() const;

	// get the socket file descriptor
	int socket_fd() const override;

	// the socket is ready for reading
	// return true if the socket should be kept, false otherwise
	void ready(ClientManager& manager) override;
};

} // namespace vanity::socket

#endif //VANITY_SOCKET_LISTENER_H
