#ifndef VANITY_ABSTRACT_SERVER_H
#define VANITY_ABSTRACT_SERVER_H

#include "client/client.h"
#include "exceptions.h"


namespace vanity{

/*
 * An AbstractServer defines the basic interface for a server
 */
class AbstractServer
{
protected:
	// max timeout in seconds for any blocking event
	static constexpr int M_MAX_TIMEOUT = 10;

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