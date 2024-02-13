//
// Created by kingsli on 9/16/23.
//

#ifndef VANITY_SOCKET_WRITER_H
#define VANITY_SOCKET_WRITER_H

#include <mutex>
#include <queue>

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

	// the mutex to protect the messages queue
	std::mutex m_mutex;

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
	void write(SocketServer& server, std::string&& response);

	// get the socket file descriptor
	int socket_fd() const override;

	// attempt to write all messages
	void ready(SocketServer& server) override;

private:
	// write to the socket
	// return false when done, true otherwise
	bool do_write();

	// try to write all messages in the queue
	void try_write_all();
};

} // namespace vanity

#endif //VANITY_SOCKET_WRITER_H
