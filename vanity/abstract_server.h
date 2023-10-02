#ifndef VANITY_ABSTRACT_SERVER_H
#define VANITY_ABSTRACT_SERVER_H

#include "queue.h"
#include "socket/socket.h"


namespace vanity{

// the constants used by the server
struct server_constants
{
	static constexpr char const* const ok = "OK";
	static constexpr char const* const error = "ERROR";
	static constexpr char const* const null = "NULL";
	static constexpr char const* const pong = "PONG";
};

// thrown from a ready() to indicate the client should be disconnected
class DestroyClient : std::exception {};

// thrown from a ready() to indicate the server should be stopped
class DestroyServer : std::exception {};

// some type of sever event that should be completed
enum class server_event{
	socket_ready,
	persist,
};

/*
 * An AbstractServer defines the basic interface for a server
 */
class AbstractServer
{
protected:
	// max timeout in seconds for any blocking event
	static constexpr int M_MAX_TIMEOUT = 10;

	// events that need attention are sent through here
	queue<server_event> m_event_queue;

public:
	AbstractServer() = default;
	virtual ~AbstractServer() = default;

	// a message was received from a client
	virtual void handle(const std::string& msg, const ClientSocket& socket) = 0;

	// send a message to a client
	virtual void send(const ClientSocket& socket, const std::string& msg) = 0;
};

} // namespace vanity

#endif // VANITY_ABSTRACT_SERVER_H