#ifndef VANITY_ABSTRACT_BASE_SERVER_H
#define VANITY_ABSTRACT_BASE_SERVER_H

#include "socket/socket.h"


namespace vanity{

/*
 * Am  AbstractBaseServer defines the basic interface for a server
 */
class AbstractBaseServer
{
public:
	AbstractBaseServer() = default;
	virtual ~AbstractBaseServer() = default;

	// a message was received from a client
	virtual void handle(const std::string& msg, const ClientSocket& socket) = 0;

	// a get instruction was received from a client
	virtual void instruction_get(const ClientSocket& socket, std::string key, std::string value) = 0;

	// a set instruction was received from a client
	virtual void instruction_set(const ClientSocket& socket, std::string key, std::string value) = 0;

	// a del instruction was received from a client
	virtual void instruction_del(const ClientSocket& socket, std::string key) = 0;
};

} // namespace vanity

#endif // VANITY_ABSTRACT_BASE_SERVER_H