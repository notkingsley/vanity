//
// Created by kingsli on 9/16/23.
//

#ifndef VANITY_SOCKET_WRITER_H
#define VANITY_SOCKET_WRITER_H


#include "socket/socket_event_handler.h"

namespace vanity {

/*
 * A SocketWriter allows to write to a socket
 */
class SocketWriter : public SocketWriteHandler
{
	// the message delimiter
	static const char m_delimiter = '~';

	// the socket to write to
	const ClientSocket& m_socket;

	// the message to write
	std::string m_message;

	// the index of the next character to write
	size_t m_index = 0;

public:
	// create a SocketWriter
	explicit SocketWriter(const ClientSocket& socket, const std::string& msg);

	// write once to the socket, stopping everytime the delimiter
	// needs to be escaped
	// return false when done, true otherwise
	bool ready(AbstractServer& server) override;

	// register to epoll for events
	void register_event(int epoll_fd) override;

	// unregister from epoll for events
	void unregister_event(int epoll_fd) override;

};

} // namespace vanity

#endif //VANITY_SOCKET_WRITER_H
