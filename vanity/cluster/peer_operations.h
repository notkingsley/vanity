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

	MAX_OP,
};

const std::initializer_list<std::pair<peer_op_t, std::string>> PEER_OP_STRINGS {
	{peer_op_t::PING,               "PING"},
	{peer_op_t::EXIT,               "EXIT"},

	{peer_op_t::PEER_AUTH,		    "PEER_AUTH"},
	{peer_op_t::PEERS,              "PEERS"},
};

// the type of the identifier of a message
using msg_id_t = int64_t;

} // namespace vanity

#endif //VANITY_PEER_OPERATIONS_H
