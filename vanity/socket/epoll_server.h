//
// Created by kingsli on 4/4/24.
//

#ifndef VANITY_EPOLL_SERVER_H
#define VANITY_EPOLL_SERVER_H

#include "client/read_manager.h"
#include "epoll.h"
#include "log_server.h"


namespace vanity::socket {

/*
 * An EpollServer handles epolls and epoll events
 */
class EpollServer:
	public virtual LogServer,
	public virtual ReadManager,
	public virtual WriteManager
{
private:
	// the epoll instance for reading
	Epoll m_read_epoll;

	// the epoll instance for writing
	Epoll m_write_epoll;

	// the epoll instance for polling the other epoll instances
	SuperEpoll m_super_epoll;

public:
	// create an epoll server
	EpollServer();

	// add a socket writer
	void add_writer(SocketWriter& writer) override;

	// remove a socket writer
	void remove_writer(SocketWriter& writer) override;

protected:
	// block until an event is ready
	// for a maximum of timeout milliseconds
	// returns the number of events ready or -1 on error
	int epoll_wait(int timeout);

	// add a SocketReadHandler
	void epoll_add(SocketReadHandler& handler);

	// remove a SocketReadHandler
	void epoll_remove(SocketReadHandler& handler);

	// pull all ready events from all epolls
	void epoll_ready();

	// a SocketReadHandler is ready
	virtual void read_ready(SocketReadHandler* handler) = 0;

private:
	// this epoll instance is ready
	void epoll_ready(Epoll& epoll);

	// an event was gotten from the read epoll
	void read_ready(epoll_event& event);

	// an event was gotten from the write epoll
	void write_ready(epoll_event& event);
};

} // namespace vanity::socket

#endif //VANITY_EPOLL_SERVER_H
