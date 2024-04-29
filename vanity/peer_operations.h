//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_PEER_OPERATIONS_H
#define VANITY_PEER_OPERATIONS_H

#include <string>

namespace vanity {

// all the possible operations that a peer can request
enum class peer_op_t {
	PING,
};

const std::initializer_list<std::pair<peer_op_t, std::string>> PEER_OP_STRINGS {
	{peer_op_t::PING, "PING"},
};

} // namespace vanity

#endif //VANITY_PEER_OPERATIONS_H
