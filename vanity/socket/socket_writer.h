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
private:
	// the socket to write to
	const ClientSocket& m_socket;

	// the messages to write
	std::queue<std::string> m_messages;

	// the index of the next character to write in the current message
	size_t m_index = 0;

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

private:
	// write to the socket
	// return false when done, true otherwise
	bool write();
};

} // namespace vanity

#endif //VANITY_SOCKET_WRITER_H
