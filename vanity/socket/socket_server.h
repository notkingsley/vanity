#ifndef VANITY_SOCKET_SERVER_H
#define VANITY_SOCKET_SERVER_H

#include <unordered_set>
#include <memory>
#include <thread>

#include "client/client_server.h"
#include "epoll_server.h"
#include "event_server.h"
#include "socket_listener.h"
#include "utils/event.h"


namespace vanity {

namespace socket {

/*
 * A SocketServer allows us to listen on one or more sockets,
 * accept connections and read/write data
 */
class SocketServer:
	public virtual ClientServer,
	public virtual EventServer
{
private:
	// active SocketConnectionServers
	std::vector<SocketListener> m_listeners;

	// all ports
	std::vector<uint16_t> m_ports;

	// the poll thread
	std::thread m_poll_thread {};

	// whether the reported socket_ready has been polled
	Event m_polled {};

	// whether the polling thread is still running
	bool m_running {false};

public:
	// create a socket server
	explicit SocketServer(std::vector<uint16_t> ports);

	// destroy the socket server
	~SocketServer() override = default;

	// no copy
	SocketServer(const SocketServer&) = delete;
	SocketServer& operator=(const SocketServer&) = delete;

	// no move
	SocketServer(SocketServer&& other) noexcept = delete;
	SocketServer& operator=(SocketServer&& other) noexcept = delete;

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

	// bind all ports
	void bind_all();
};

} // namespace socket

using socket::SocketServer;

} // namespace vanity

#endif // VANITY_SOCKET_SERVER_H