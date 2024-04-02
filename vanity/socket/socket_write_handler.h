//
// Created by kingsli on 4/2/24.
//

#ifndef VANITY_SOCKET_WRITE_HANDLER_H
#define VANITY_SOCKET_WRITE_HANDLER_H

#include <string>
#include <sys/epoll.h>

namespace vanity::socket {

// forward declaration
class SocketServer;

/*
 * A SocketWriteHandler is the common interface for registering for write events
*/
class SocketWriteHandler
{
public:
	// get write event bit mask
	static constexpr uint32_t get_event_mask() {
		return EPOLLOUT;
	};

	virtual ~SocketWriteHandler() = default;

	// get the socket file descriptor
	virtual int socket_fd() const = 0;

	// the registered event has occurred
	// return true if the handler should be kept, false otherwise
	virtual void ready(SocketServer& server) = 0;
};

} // namespace vanity::socket

#endif //VANITY_SOCKET_WRITE_HANDLER_H
