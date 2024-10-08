//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_PEER_OPERATIONS_H
#define VANITY_PEER_OPERATIONS_H

#include <string>

namespace vanity {

// all the possible operations that a peer can request
enum class peer_op_t : uint {
	PING,
	EXIT,

	PEER_AUTH,
	PEERS,

	ASK_EVICT,

	MAX_OP,
};

// the strings that represent the operations
const std::initializer_list<std::pair<peer_op_t, std::string>> PEER_OP_STRINGS {
	{peer_op_t::PING,               "PING"},
	{peer_op_t::EXIT,               "EXIT"},

	{peer_op_t::PEER_AUTH,		    "PEER_AUTH"},
	{peer_op_t::PEERS,              "PEERS"},

	{peer_op_t::ASK_EVICT,          "ASK_EVICT"},
};

// all the possible async operations that a peer can send
enum class async_op_t : uint {
	PULSE,

	MAX_OP,
};

// the strings that represent the async operations
const std::initializer_list<std::pair<async_op_t, std::string>> ASYNC_OP_STRINGS {
	{async_op_t::PULSE,             "PULSE"},
};

// the type of the identifier of a message
using msg_id_t = int64_t;

} // namespace vanity

#endif //VANITY_PEER_OPERATIONS_H
