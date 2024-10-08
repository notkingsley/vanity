//
// Created by kingsli on 4/28/24.
//

#include "peer_request.h"

namespace vanity {

PeerRequest::PeerRequest(const std::string &msg) : Extractable(msg) {}

peer_request_t PeerRequest::get_request_t() {
	return get_from_list(REQUEST_STRINGS, "invalid request type");
}

ReplyStatus PeerRequest::get_reply_status() {
	return get_from_list(REPLY_STATUS_STRINGS, "invalid reply status");
}

peer_op_t PeerRequest::get_op() {
	return get_from_list(PEER_OP_STRINGS, "invalid peer operation");
}

async_op_t PeerRequest::get_async_op() {
	return get_from_list(ASYNC_OP_STRINGS, "invalid async operation");
}

std::string PeerRequest::format() const {
	return m_msg + " at " + std::to_string(m_pos);
}

} // namespace vanity
