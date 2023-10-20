//
// Created by kingsli on 9/16/23.
//

#ifndef VANITY_SOCKET_WRITER_H
#define VANITY_SOCKET_WRITER_H

#include <queue>

#include "socket_event_handler.h"


namespace vanity {

/*
 * A MessageWriter can write a single message to a socket
 */
class MessageWriter
{
private:
	// the message delimiter
	static const char m_delimiter = '~';

	// the message to write
	std::string m_message;

	// the index of the next character to write
	size_t m_index = 0;

public:
	// create a MessageWriter
	explicit MessageWriter(const std::string& msg);

	// write once to the socket, stopping everytime the delimiter
	// needs to be escaped
	// return false when done, true otherwise
	bool write(const ClientSocket& socket);
};

/*
 * A SocketWriter allows to write to a socket
 */
class SocketWriter : public SocketWriteHandler
{
	// the socket to write to
	const ClientSocket& m_socket;

	// the message_writers
	std::queue<MessageWriter> m_message_writers;

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

	// register to the server's epoll to write msg
	void register_write(SocketServer& server, const std::string& msg);

	// attempt to write all messages
	void ready(SocketServer& server) override;

	// register to epoll for events
	void register_event(int epoll_fd) override;

	// unregister from epoll for events
	void unregister_event(int epoll_fd) const override;
};

} // namespace vanity

#endif //VANITY_SOCKET_WRITER_H
