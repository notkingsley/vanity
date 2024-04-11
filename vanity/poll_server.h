//
// Created by kingsli on 4/11/24.
//

#ifndef VANITY_POLL_SERVER_H
#define VANITY_POLL_SERVER_H

#include "event_server.h"
#include "socket/epoll_server.h"
#include "utils/event.h"


namespace vanity {

/*
 * A PollServer add liveness to the EpollServer
 * by waiting on the epoll on a separate thread
 * and pushing events to be handled by worker
 * threads when an event is ready
 */
class PollServer:
	public virtual EventServer,
	public virtual socket::EpollServer
{
private:
	// the poll thread
	std::thread m_poll_thread {};

	// whether the reported socket_ready has been polled
	Event m_polled {};

	// whether the polling thread is still running
	bool m_running {false};

public:
	// destroy the poll server
	~PollServer() override = default;

	// a socket_ready event was received
	void event_socket_ready() override;

protected:
	// start polling
	void start();

	// stop polling
	void stop();

private:
	// block on epoll on a separate thread.
	// yield server_event::socket_ready to the
	// event queue when some socket is ready
	void poll();
};

} // namespace vanity

#endif //VANITY_POLL_SERVER_H
