#ifndef VANITY_SOCKET_SERVER_H
#define VANITY_SOCKET_SERVER_H

#include <unordered_set>
#include <memory>

#include "abstract_base_server.h"
#include "socket.h"


namespace vanity{

/*
 * A SocketServer allows us to listen on one or more sockets, accept connections and read/write data
 */
class SocketServer : public AbstractBaseServer
{
private:
	// max number of events to process at once
	static constexpr int MAX_EVENTS = 10;

	// epoll timeout in milliseconds
	static constexpr int EPOLL_TIMEOUT = 1000;

	// epoll events array
	epoll_event m_events[MAX_EVENTS] {};

	// epoll file descriptor
	int m_epoll_fd;

	// the current set of handlers
	std::unordered_set<std::unique_ptr<SocketEventHandler>> m_handlers;

public:
	// create a socket server
	SocketServer();

	// destroy the socket server
	~SocketServer() override;

	// no copy
	SocketServer(const SocketServer&) = delete;
	SocketServer& operator=(const SocketServer&) = delete;

	// move constructor
	SocketServer(SocketServer&& other) noexcept;
	SocketServer& operator=(SocketServer&& other) noexcept;

	// add a handler
	void add_handler(std::unique_ptr<SocketEventHandler>&& handler);

	// remove a handler
	void remove_handler(SocketEventHandler& handler);

	// start listening on port
	void listen(uint16_t port);

	// start the server
	void start();

	// a message has been received from a client
	void handle(const std::string& msg, const ClientSocket& socket) override;
};

} // namespace vanity

#endif // VANITY_SOCKET_SERVER_H