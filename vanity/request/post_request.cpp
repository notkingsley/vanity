//
// Created by kingsli on 4/28/24.
//

#include "post_request.h"

namespace vanity {

PostRequest::PostRequest(const PeerRequest &request): Extractable{request} {}

peer_op_t PostRequest::get_op() {
	return get_from_list(PEER_OP_STRINGS, "invalid peer operation");
}

} // namespace vanity
