//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_INSTRUCTION_SERVER_H
#define VANITY_INSTRUCTION_SERVER_H


#include "abstract_base_server.h"


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
class InstructionServer : public AbstractBaseServer
{
public:
	// a message was received from a client
	void handle(const std::string& msg, const ClientSocket& socket) override;
};

} // namespace vanity

#endif //VANITY_INSTRUCTION_SERVER_H
