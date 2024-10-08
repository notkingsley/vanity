//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_POST_MESSAGE_H
#define VANITY_POST_MESSAGE_H

#include <array>

#include "cluster/peer_operations.h"
#include "sendable.h"

namespace vanity {

/*
 * A PostMessage is a Sendable for composing a new message
 * to be sent to a peer
 */
class PostMessage : public Sendable
{
private:
	// the number of peer_op_t values
	static constexpr auto M_OP_COUNT = static_cast<uint>(peer_op_t::MAX_OP);

	// make an array indexable by peer_op_t to get the string representation
	static std::array<const char*, M_OP_COUNT> init_op_strings();

	// the string representations of the peer_op_t values
	static const std::array<const char*, M_OP_COUNT> M_OP_STRINGS;

	// a type to create a PostMessage to be sent as a plain user
	struct as_plain_user_t { };

public:
	// only instance of as_plain_user_t
	static constexpr auto as_plain_user = as_plain_user_t{};

	// create a new PostMessage
	PostMessage();

	// create a new PostMessage to be sent as a plain user
	explicit PostMessage(as_plain_user_t);

	// get the string representation of a peer_op_t
	static const char* op_to_string(peer_op_t op);

	// add a peer_op_t to the message
	PostMessage& operator<<(peer_op_t op);

	// add a message to the message
	PostMessage& operator<<(const std::string& data);

	// add a message to the message
	PostMessage& operator<<(const char* data);

	// add a message to the message
	PostMessage& operator<<(char data);

	// serialize a string to the message
	PostMessage& serialize(const std::string& data);

	// serialize an int64_t to a Response
	PostMessage& serialize(int64_t data);

	// serialize a length to the message
	PostMessage& serialize_length(size_t length);
};

} // namespace vanity

#endif //VANITY_POST_MESSAGE_H
