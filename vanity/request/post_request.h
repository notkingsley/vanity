//
// Created by kingsli on 4/28/24.
//

#ifndef VANITY_POST_REQUEST_H
#define VANITY_POST_REQUEST_H

#include "peer_operations.h"
#include "peer_request.h"

namespace vanity {

/*
 * A PostRequest is a request that a peer has sent
 */
class PostRequest : public Extractable
{
public:
	// create a PostRequest with a PeerRequest and an identifier
	explicit PostRequest(const PeerRequest& request);

	// extract a peer operation from the request
	peer_op_t get_op();
};

} // namespace vanity

#endif //VANITY_POST_REQUEST_H
