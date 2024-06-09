//
// Created by kingsli on 6/10/24.
//

#ifndef VANITY_ASYNC_MESSAGE_H
#define VANITY_ASYNC_MESSAGE_H

#include <array>

#include "cluster/peer_operations.h"
#include "sendable.h"


namespace vanity {

/*
 * An AsyncMessage is a Sendable for composing an async message
 * to be sent to a peer
 */
class AsyncMessage : public Sendable
{
private:
	// the number of async_op_t values
	static constexpr auto M_OP_COUNT = static_cast<uint>(async_op_t::MAX_OP);

	// make an array indexable by async_op_t to get the string representation
	static std::array<const char*, M_OP_COUNT> init_op_strings();

	// the string representations of the async_op_t values
	static const std::array<const char*, M_OP_COUNT> M_ASYNC_OP_STRINGS;

public:
	// create a new AsyncMessage
	AsyncMessage();

	// add an async_op_t to the message
	AsyncMessage& operator<<(async_op_t op);

	// add a string to the message
	AsyncMessage& operator<<(const std::string& data);

	// add a c-string to the message
	AsyncMessage& operator<<(const char* data);

	// add a character to the message
	AsyncMessage& operator<<(char data);

	// serialize a string to the message
	AsyncMessage& serialize(const std::string& data);

	// serialize an int64_t to a Response
	AsyncMessage& serialize(int64_t data);

	// serialize a length to the message
	AsyncMessage& serialize_length(size_t length);
};

} // namespace vanity

#endif //VANITY_ASYNC_MESSAGE_H
