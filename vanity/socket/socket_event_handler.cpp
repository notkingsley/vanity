//
// Created by kingsli on 9/6/23.
//

#include "socket.h"
#include "socket_event_handler.h"


namespace vanity{

void SocketEventHandler::register_event(int epoll_fd, int socket_fd)
{
	epoll_event event{};
	event.events = get_event_mask();
	event.data.ptr = this;
	int ctl = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);
	if (ctl < 0)
		throw SocketError("Could not add client to epoll");
}

void SocketEventHandler::unregister_event(int epoll_fd, int socket_fd) {
	int ctl = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket_fd, nullptr);
	if (ctl < 0)
		throw SocketError("Could not remove client from epoll");
}

}