//
// Created by kingsli on 10/8/23.
//

#ifndef VANITY_SOCKET_CLIENT_H
#define VANITY_SOCKET_CLIENT_H

#include "abstract_server.h"
#include "socket.h"
#include "socket_event_handler.h"
#include "socket_reader.h"


namespace vanity {

/*
 * A Client is a handle for an active connection,
 * and a handler for socket read events
 */
class Client : public SocketReadHandler
{
private:
	// the socket to read from
	ClientSocket m_socket;

	// the reader for the socket
	SocketReader m_reader;

public:
	// create a client
	explicit Client(ClientSocket&& socket) : m_socket(std::move(socket)) {};

	// get the socket
	const ClientSocket& socket() const
	{
		return m_socket;
	}

	// the client has sent a message, and it is ready to be read
	bool ready(AbstractServer& server) override
	{
		return m_reader.read(server, *this);
	}

	// register to epoll for events
	void register_event(int epoll_fd) override
	{
		m_socket.register_event(epoll_fd, *this);
	}

	// unregister from epoll for events
	void unregister_event(int epoll_fd) override
	{
		m_socket.unregister_event(epoll_fd);
	}
};

} // namespace vanity

#endif //VANITY_SOCKET_CLIENT_H
