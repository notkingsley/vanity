//
// Created by kingsli on 4/29/24.
//

#include "post_request_server.h"

namespace vanity {

void PostRequestServer::handle_post_request(PostRequest &request, Client &client) {
	using enum object_t;

	auto id = request.get<INT>();
	auto op = request.get_op();
	Context ctx {id, client};
	bool end = true;

	switch (op) {
		case peer_op_t::PING: {
			post_request_ping(ctx);
			break;
		}

		case peer_op_t::PEER_AUTH: {
			auto [key, addr] = request.get_exact<STR, STR>(end);
			post_request_peer_auth(ctx, key, addr);
			break;
		}

		case peer_op_t::MAX_OP: {
			// TODO: report peer
		}
	}
}

void PostRequestServer::post_request_ping(Context& ctx) {
	reply(ctx, PONG);
}

} // namespace vanity
