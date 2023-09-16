#ifndef VANITY_ABSTRACT_SERVER_H
#define VANITY_ABSTRACT_SERVER_H

#include "socket/socket.h"


namespace vanity{

// the constants used by the server
struct server_constants
{
	static constexpr char const* const ok = "OK";
	static constexpr char const* const error = "ERROR";
	static constexpr char const* const null = "NULL";
};

/*
 * An AbstractServer defines the basic interface for a server
 */
class AbstractServer
{
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