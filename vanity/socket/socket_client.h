//
// Created by kingsli on 10/8/23.
//

#ifndef VANITY_SOCKET_CLIENT_H
#define VANITY_SOCKET_CLIENT_H

#include "socket.h"
#include "socket_event_handler.h"
#include "socket_reader.h"


namespace vanity {

/*
 * A SocketClient is a handle for an active connection,
 * and a handler for socket read events
 */
class SocketClient : public SocketReadHandler
{
protected:
	// the socket to read from
	ClientSocket m_socket;

	// the reader for the socket
	SocketReader m_reader;

public:
	// create a client
	explicit SocketClient(ClientSocket&& socket) : m_socket(std::move(socket)) {};

	// get the socket
	const ClientSocket& socket() const
	{
		return m_socket;
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
