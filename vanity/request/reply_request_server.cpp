//
// Created by kingsli on 5/3/24.
//

#include "reply_request_server.h"

namespace vanity {

void ReplyRequestServer::handle_reply_request(ReplyRequest &request, Client &client) {
	auto id = request.get<object_t::INT>();
	auto op = expected_op(id);
	if (not op) {
		// TODO: report peer
	}

	switch (*op) {
		case peer_op_t::PING: {
			reply_request_ping(client);
			break;
		}

		case peer_op_t::MAX_OP: {
			// TODO: report peer
		}
	}
}

void ReplyRequestServer::reply_request_ping(Client &client) {
	// TODO what to do when a ping reply is received? commend the peer?
}

} // namespace vanity
