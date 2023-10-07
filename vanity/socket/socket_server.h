#ifndef VANITY_SOCKET_SERVER_H
#define VANITY_SOCKET_SERVER_H

#include <unordered_set>
#include <memory>
#include <thread>

#include "abstract_server.h"
#include "epoll.h"
#include "event.h"
#include "logging.h"
#include "socket.h"
#include "socket_event_handler.h"


namespace vanity{

/*
 * A SocketServer allows us to listen on one or more sockets,
 * accept connections and read/write data
 */
class SocketServer : public virtual AbstractServer, public virtual Logger
{
private:
	// the current set of handlers
	std::unordered_set<std::unique_ptr<SocketEventHandler>> m_handlers;

	// the poll thread
	std::thread m_poll_thread {};

	// whether the reported socket_ready has been polled
	event m_polled {};

	// the epoll instance
	Epoll m_epoll;

	// whether the polling thread is still running
	bool m_running {false};

public:
	// create a socket server
	SocketServer() {};

	// destroy the socket server
	~SocketServer() override = default;

	// no copy
	SocketServer(const SocketServer&) = delete;
	SocketServer& operator=(const SocketServer&) = delete;

	// no move
	SocketServer(SocketServer&& other) noexcept = delete;
	SocketServer& operator=(SocketServer&& other) noexcept = delete;

	// add a handler
	void add_socket_handler(std::unique_ptr<SocketEventHandler>&& handler);

	// remove a handler
	void remove_socket_handler(SocketEventHandler& handler);

	// bind to and start listening on port
	void bind(uint16_t port);

	// send a message to a client
	void send(const ClientSocket& client, const std::string& msg) override;

protected:
	// start polling as a background task
	void start();

	// some polled socket is ready
	void socket_ready();

	// the server is being exited. this stops and joins the poll thread
	// and deletes the socket handlers (and the sockets themselves)
	void stop();

private:
	// block on epoll on a separate thread.
	// yield server_event::socket_ready to the
	// event queue when some socket is ready
	void poll();
};

} // namespace vanity

#endif // VANITY_SOCKET_SERVER_H