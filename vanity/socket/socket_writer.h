//
// Created by kingsli on 9/16/23.
//

#ifndef VANITY_SOCKET_WRITER_H
#define VANITY_SOCKET_WRITER_H

#include <mutex>
#include <queue>

#include "socket_write_handler.h"


namespace vanity::socket {

/*
 * A SocketWriter allows to write to a socket
 */
class SocketWriter : public SocketWriteHandler
{
private:
	// the socket to write to
	const Socket& m_socket;

	// the message to write
	std::string m_message;

	// the mutex to protect the message string
	std::mutex m_mutex;

	// the index of the next character to write in the current message
	size_t m_index = 0;

public:
	// create a SocketWriter
	explicit SocketWriter(const Socket& socket);

	// move constructor
	SocketWriter(SocketWriter&& other) noexcept = delete;

	// move assignment
	SocketWriter& operator=(SocketWriter&& other) noexcept = delete;

	// copy constructor
	SocketWriter(const SocketWriter& other) = delete;

	// copy assignment
	SocketWriter& operator=(const SocketWriter& other) = delete;

	// register to the server's epoll to write a response
	void write(WriteManager& manager, std::string&& response);

	// get the socket file descriptor
	int socket_fd() const override;

	// attempt to write all messages
	void ready(WriteManager& manager) override;

private:
	// write to the socket
	// return false when done, true otherwise
	bool do_write();

	// try to write all messages in the queue
	void try_write_all();
};

} // namespace vanity::socket

#endif //VANITY_SOCKET_WRITER_H
