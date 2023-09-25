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
	explicit InvalidInstruction(std::string msg) : m_msg{"Invalid Instruction: " + std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

/*
 * A InstructionServer allows to handle incoming instructions
 * and dispatch them to the appropriate handler
 */
class InstructionServer : public virtual AbstractServer
{
public:
	// prepare a string to be sent
	static std::string prepare(const std::string& msg);

	// prepare a numeric value to be sent
	template<typename T>
	static std::enable_if_t<std::is_arithmetic_v<T>, std::string> prepare(T value){
		return std::to_string(value);
	}

	// a message was received from a client
	void handle(const std::string& msg, const ClientSocket& socket) override;

	// a get instruction was received from a client
	virtual void instruction_get(const ClientSocket& socket, const std::string& key) = 0;

	// a set instruction was received from a client
	virtual void instruction_set(const ClientSocket& socket, const std::string& key, const std::string& value) = 0;

	// a del instruction was received from a client
	virtual void instruction_del(const ClientSocket& socket, const std::string& key) = 0;

	// a persist instruction was received from a client
	virtual void instruction_persist(const ClientSocket& socket) = 0;

	// a reset instruction was received from a client
	virtual void instruction_reset(const ClientSocket& socket) = 0;

	// an exit instruction was received from a client
	virtual void instruction_exit(const ClientSocket& socket) {
		throw DestroyClient{};
	};

	// a terminate instruction was received from a client
	virtual void instruction_terminate(const ClientSocket& socket) {
		throw DestroyServer{};
	};

	// a ping instruction was received from a client
	virtual void instruction_ping(const ClientSocket& socket, const std::string& msg) {
		send(socket, server_constants::pong + msg);
	};
};

} // namespace vanity

#endif //VANITY_INSTRUCTION_SERVER_H
