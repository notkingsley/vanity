//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_INSTRUCTION_SERVER_H
#define VANITY_INSTRUCTION_SERVER_H


#include "abstract_server.h"


namespace vanity {

// the message received was invalid
class InvalidInstruction : std::exception
{
private:
	std::string m_msg;

public:
	explicit InvalidInstruction(std::string  msg) : m_msg{"Invalid Instruction" + std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

/*
 * A InstructionServer allows to handle incoming instructions
 * and dispatch them to the appropriate handler
 */
class InstructionServer : public virtual AbstractServer
{
public:
	// a message was received from a client
	void handle(const std::string& msg, const ClientSocket& socket) override;

	// a get instruction was received from a client
	virtual void instruction_get(const ClientSocket& socket, const std::string& key) = 0;

	// a set instruction was received from a client
	virtual void instruction_set(const ClientSocket& socket, const std::string& key, const std::string& value) = 0;

	// a del instruction was received from a client
	virtual void instruction_del(const ClientSocket& socket, const std::string& key) = 0;
};

} // namespace vanity

#endif //VANITY_INSTRUCTION_SERVER_H
