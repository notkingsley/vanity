//
// Created by kingsli on 9/6/23.
//

#ifndef VANITY_SOCKET_EVENT_HANDLER_H
#define VANITY_SOCKET_EVENT_HANDLER_H


#include <string>
#include <sys/epoll.h>

namespace vanity{

// forward declaration
class SocketServer;

/*
 * A SocketEventHandler is the common interface for registering for events
 */
class SocketEventHandler
{
public:
	virtual ~SocketEventHandler() = default;

	// get the event bit mask
	virtual uint32_t get_event_mask() const = 0;

	// get the socket file descriptor
	virtual int socket_fd() const = 0;

	// the registered event has occurred
	// return true if the handler should be kept, false otherwise
	virtual void ready(SocketServer& server) = 0;
};

/*
 * A SocketReadHandler is the common interface for registering for read events
*/
class SocketReadHandler : public SocketEventHandler
{
public:
	// get read event bit mask
	uint32_t get_event_mask() const override { return EPOLLIN; };
};

/*
 * A SocketWriteHandler is the common interface for registering for write events
*/
class SocketWriteHandler : public SocketEventHandler
{
public:
	// get write event bit mask
	uint32_t get_event_mask() const override { return EPOLLOUT; };
};

} // namespace vanity

#endif //VANITY_SOCKET_EVENT_HANDLER_H
