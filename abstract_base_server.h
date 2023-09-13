#ifndef VANITY_ABSTRACT_BASE_SERVER_H
#define VANITY_ABSTRACT_BASE_SERVER_H

#include "instruction.h"
#include "socket.h"


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

	// handle an instruction
	virtual void handle(const instruction_t& instruction, const ClientSocket& socket) = 0;
};

} // namespace vanity

#endif // VANITY_ABSTRACT_BASE_SERVER_H