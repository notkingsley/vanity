//
// Created by kingsli on 9/6/23.
//

#ifndef VANITY_SOCKET_EVENT_HANDLER_H
#define VANITY_SOCKET_EVENT_HANDLER_H


#include <string>
#include <sys/epoll.h>

#include "abstract_server.h"

namespace vanity{

/*
 * A SocketEventHandler is the common interface for registering for events
 */
class SocketEventHandler
{
public:
	virtual ~SocketEventHandler() = default;

	// register to epoll for events
	void register_event(int epoll_fd, int socket_fd);

	// register to epoll for events
	virtual void register_event(int epoll_fd) = 0;

	// unregister from epoll for events
	static void unregister_event(int epoll_fd, int socket_fd);

	// unregister from epoll for events
	virtual void unregister_event(int epoll_fd) = 0;

	// get the event bit mask
	virtual uint32_t get_event_mask() const = 0;

	// the registered event has occurred
	// return true if the handler should be kept, false otherwise
	virtual bool ready(AbstractServer& server) = 0;
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
