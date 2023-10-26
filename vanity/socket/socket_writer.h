//
// Created by kingsli on 9/16/23.
//

#ifndef VANITY_SOCKET_WRITER_H
#define VANITY_SOCKET_WRITER_H

#include <queue>

#include "response.h"
#include "socket_event_handler.h"


namespace vanity {

/*
 * A SocketWriter allows to write to a socket
 */
class SocketWriter : public SocketWriteHandler
{
	// the socket to write to
	const ClientSocket& m_socket;

	// the message_writers
	std::queue<Response> m_responses;

public:
	// create a SocketWriter
	explicit SocketWriter(const ClientSocket& socket);

	// move constructor
	SocketWriter(SocketWriter&& other) noexcept;

	// move assignment
	SocketWriter& operator=(SocketWriter&& other) noexcept = delete;

	// copy constructor
	SocketWriter(const SocketWriter& other);

	// copy assignment
	SocketWriter& operator=(const SocketWriter& other) = delete;

	// register to the server's epoll to write a response
	void register_write(SocketServer& server, Response&& response);

	// attempt to write all messages
	void ready(SocketServer& server) override;

	// register to epoll for events
	void register_event(int epoll_fd) override;

	// unregister from epoll for events
	void unregister_event(int epoll_fd) const override;
};

} // namespace vanity

#endif //VANITY_SOCKET_WRITER_H
