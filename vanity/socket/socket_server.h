#ifndef VANITY_SOCKET_SERVER_H
#define VANITY_SOCKET_SERVER_H

#include <unordered_set>
#include <memory>
#include <thread>

#include "client/concrete_client.h"
#include "epoll.h"
#include "event_server.h"
#include "log_server.h"
#include "socket_connection_server.h"
#include "utils/event.h"


namespace vanity {

namespace socket {

// forward declaration
class SocketConnectionServer;

/*
 * A SocketServer allows us to listen on one or more sockets,
 * accept connections and read/write data
 */
class SocketServer:
	public virtual EventServer,
	public virtual LogServer,
	public virtual WriteManager
{
private:
	// the current set of clients
	std::unordered_set<ConcreteClient> m_clients;

	// active SocketConnectionServers
	std::vector<SocketConnectionServer> m_connection_servers;

	// all ports
	std::vector<uint16_t> m_ports;

	// the poll thread
	std::thread m_poll_thread {};

	// whether the reported socket_ready has been polled
	Event m_polled {};

	// the epoll instance for reading
	Epoll m_read_epoll;

	// the epoll instance for writing
	Epoll m_write_epoll;

	// the epoll instance for polling the other epoll instances
	SuperEpoll m_super_epoll;

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

	// send a message to a client
	void send(Client& client, Response&& response) override;

	// add a new client
	void add_client(ConcreteClient&& client);

	// remove a client
	// this is virtual because it's the most pragmatic solution
	// without introducing hooks or callbacks which are unnecessary
	// for now. this should be temporary, however.
	virtual void remove_client(ConcreteClient& client);

	// add a socket writer
	void add_writer(SocketWriter& writer) override;

	// remove a socket writer
	void remove_socket_writer(SocketWriter& writer);

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

	// this epoll instance is ready
	void epoll_ready(Epoll& epoll);

	// bind all ports
	void bind_all();
};

} // namespace socket

using socket::SocketServer;

} // namespace vanity

#endif // VANITY_SOCKET_SERVER_H