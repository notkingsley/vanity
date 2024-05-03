//
// Created by kingsli on 4/29/24.
//

#include "post_request_server.h"

namespace vanity {

void PostRequestServer::handle_post_request(PostRequest &request, Client &client) {
	auto id = request.get<object_t::INT>();
	auto op = request.get_op();
	Context ctx {id, client};

	switch (op) {
		case peer_op_t::PING: {
			post_request_ping(ctx);
			break;
		}

		case peer_op_t::MAX_OP: {
			// TODO: report peer
		}
	}
}

void PostRequestServer::post_request_ping(Context& ctx) {
	// TODO: send pong
}

} // namespace vanity
