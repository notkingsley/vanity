//
// Created by kingsli on 4/28/24.
//

#ifndef VANITY_PEER_REQUEST_H
#define VANITY_PEER_REQUEST_H

#include "cluster/peer_operations.h"
#include "extractable.h"


namespace vanity {

// the kinds of messages a peer can send
enum class peer_request_t {
	POST,
	REPLY,
};

const std::initializer_list<std::pair<peer_request_t, std::string>> REQUEST_STRINGS {
	{peer_request_t::POST,  "POST"},
	{peer_request_t::REPLY, "REPLY"},
};

/*
 * A PeerRequest represents a message from a peer
 *
 * This is essentially a transitional class that
 * will be used to create a PostRequest or ReplyRequest
 */
class PeerRequest : public Extractable
{
public:
	// create a PeerRequest with a request string
	explicit PeerRequest(const std::string& msg);

	// get the type of the request
	peer_request_t get_request_t();

	// extract a peer operation from the request
	peer_op_t get_op();
};

} // namespace vanity

#endif //VANITY_PEER_REQUEST_H
