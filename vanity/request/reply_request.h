//
// Created by kingsli on 5/3/24.
//

#ifndef VANITY_REPLY_REQUEST_H
#define VANITY_REPLY_REQUEST_H

#include "peer_request.h"

namespace vanity {

/*
 * A ReplyRequest is a reply to a post request that we sent to a peer
 */
class ReplyRequest : public Extractable
{
public:
	// create a ReplyRequest with a PeerRequest and an identifier
	explicit ReplyRequest(const PeerRequest& request);
};

} // namespace vanity

#endif //VANITY_REPLY_REQUEST_H
