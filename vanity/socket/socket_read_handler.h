//
// Created by kingsli on 4/2/24.
//

#ifndef VANITY_SOCKET_READ_HANDLER_H
#define VANITY_SOCKET_READ_HANDLER_H

#include <sys/epoll.h>

#include "client/read_manager.h"


namespace vanity::socket {

/*
 * A SocketReadHandler is the common interface for registering for read events
*/
class SocketReadHandler
{
public:
	// get read event bit mask
	static constexpr uint32_t get_event_mask() {
		return EPOLLIN;
	};

	virtual ~SocketReadHandler() = default;

	// get the socket file descriptor
	virtual int socket_fd() const = 0;

	// the registered event has occurred
	// return true if the handler should be kept, false otherwise
	virtual void ready(ReadManager& manager) = 0;
};

} // namespace vanity::socket

#endif //VANITY_SOCKET_READ_HANDLER_H
