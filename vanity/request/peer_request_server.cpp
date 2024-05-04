//
// Created by kingsli on 4/29/24.
//

#include "peer_request_server.h"
#include "post_request.h"
#include "reply_request.h"


namespace vanity {

void PeerRequestServer::handle_peer(const std::string &msg, Client &client) {
	PeerRequest request{msg};
	auto request_t = request.get_request_t();

	switch (request_t) {
		case peer_request_t::POST: {
			handle_post_request(request, client);
			break;
		}
		case peer_request_t::REPLY: {
			handle_reply_request(request, client);
			break;
		}
	}
}

void PeerRequestServer::post_request_ping(Context& ctx) {
	reply(ctx, PONG);
}

void PeerRequestServer::reply_request_ping(Context& ctx) { }

void PeerRequestServer::handle_post_request(PeerRequest& peer_request, Client &client) {
	using enum object_t;

	PostRequest request{peer_request};
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

void PeerRequestServer::handle_reply_request(PeerRequest& peer_request, Client &client) {
	using enum object_t;

	ReplyRequest request{peer_request};
	auto id = request.get<INT>();
	auto op = expected_op(id);
	Context ctx {id, client};
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

		case peer_op_t::MAX_OP: {
			// TODO: report peer
		}
	}
}

} // namespace vanity
