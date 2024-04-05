#ifndef VANITY_SOCKET_SERVER_H
#define VANITY_SOCKET_SERVER_H

#include <unordered_set>
#include <memory>
#include <thread>

#include "client/tcp_client.h"
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
	public virtual ClientManager,
	public virtual EpollServer,
	public virtual EventServer
{
private:
	// the current set of clients
	std::unordered_set<TcpClient> m_clients;

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

	// send a message to a client
	void send(Client& client, Response&& response) override;

	// add a new client
	void add_client(TcpClient&& client) override;

	// remove a client
	void remove_client(TcpClient& client) override;

	// get a callback for when a message is received
	handle_callback_t handle_callback(TcpClient& client) override;

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

	// cast this to a ClientManager
	ClientManager& as_client_manager();

	// cast this to a ReadManager
	ReadManager& as_read_manager();

	// bind all ports
	void bind_all();

	void read_handler_ready(SocketReadHandler *handler) override;
};

} // namespace socket

using socket::SocketServer;

} // namespace vanity

#endif // VANITY_SOCKET_SERVER_H