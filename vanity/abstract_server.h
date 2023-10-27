#ifndef VANITY_ABSTRACT_SERVER_H
#define VANITY_ABSTRACT_SERVER_H

#include "client.h"
#include "exceptions.h"
#include "queue.h"


namespace vanity{

// some type of sever event that should be completed
enum class server_event{
	socket_ready,
	persist,
	terminate,
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

	// move constructor
	AbstractServer(AbstractServer&& other) noexcept = default;
	AbstractServer& operator=(AbstractServer&& other) noexcept = default;

	// a message was received from a client
	virtual void handle(const std::string& msg, Client& client) = 0;

	// send a response to a client
	virtual void send(Client& client, Response&& response) = 0;

	// request the server to terminate
	virtual void terminate() = 0;
};

} // namespace vanity

#endif // VANITY_ABSTRACT_SERVER_H