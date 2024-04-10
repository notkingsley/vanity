#ifndef VANITY_SOCKET_SERVER_H
#define VANITY_SOCKET_SERVER_H

#include <unordered_set>
#include <memory>
#include <thread>

#include "bind_server.h"
#include "client/client_server.h"
#include "event_server.h"
#include "utils/event.h"


namespace vanity {

namespace socket {

/*
 * A SocketServer allows us to listen on one or more sockets,
 * accept connections and read/write data
 */
class SocketServer:
	public virtual BindServer,
	public virtual ClientServer,
	public virtual EventServer
{
private:
	// the poll thread
	std::thread m_poll_thread {};

	// whether the reported socket_ready has been polled
	Event m_polled {};

	// whether the polling thread is still running
	bool m_running {false};

public:
	// destroy the socket server
	~SocketServer() override = default;

	// a socket_ready event was received
	void event_socket_ready() override;

protected:
	// start listening on all ports and start polling as a background task
	void start();

	// the server is being exited. this stops and joins the poll thread
	// and deletes the socket handlers (and the sockets themselves)
	void stop();

private:
	// block on epoll on a separate thread.
	// yield server_event::socket_ready to the
	// event queue when some socket is ready
	void poll();
};

} // namespace socket

using socket::SocketServer;

} // namespace vanity

#endif // VANITY_SOCKET_SERVER_H