//
// Created by kingsli on 4/28/24.
//

#include "peer_request.h"

namespace vanity {

PeerRequest::PeerRequest(const std::string &msg) : Extractable(msg) {}

peer_request_t PeerRequest::get_request_t() {
	return get_from_list(REQUEST_STRINGS, "invalid request type");
}

} // namespace vanity
