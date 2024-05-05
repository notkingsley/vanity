//
// Created by kingsli on 4/29/24.
//

#include "peer_request_server.h"


namespace vanity {

void PeerRequestServer::handle_peer(const std::string &msg, Client &client) {
	PeerRequest request{msg};
	auto request_t = request.get_request_t();
	auto id = request.get<object_t::INT>();
	Context ctx {id, client};

	switch (request_t) {
		case peer_request_t::POST: {
			handle_post_request(request, ctx);
			break;
		}
		case peer_request_t::REPLY: {
			handle_reply_request(request, ctx);
			break;
		}
	}
}

void PeerRequestServer::post_request_ping(Context& ctx) {
	reply(ctx, PONG);
}

void PeerRequestServer::reply_request_ping(Context& ctx) { }

void PeerRequestServer::handle_post_request(PeerRequest& request, Context& ctx) {
	using enum object_t;

	auto op = request.get_op();
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
		case peer_op_t::PEERS: {
			post_request_peers(ctx);
			break;
		}

		case peer_op_t::MAX_OP: {
			// TODO: report peer
		}
	}
}

void PeerRequestServer::handle_reply_request(PeerRequest& request, Context& ctx) {
	using enum object_t;

	auto op = expected_op(ctx.msg_id);
	bool end = true;

	if (not op) {
		// TODO: report peer
	}

	switch (*op) {
		case peer_op_t::PING: {
			reply_request_ping(ctx);
			break;
		}

		case peer_op_t::PEER_AUTH: {
			auto data = request.get_exact<STR>(end);
			reply_request_peer_auth(ctx, data);
			break;
		}
		case peer_op_t::PEERS: {
			auto peers = request.get_exact<SET>(end);
			reply_request_peers(ctx, peers);
			break;
		}

		case peer_op_t::MAX_OP: {
			// TODO: report peer
		}
	}
}

} // namespace vanity
