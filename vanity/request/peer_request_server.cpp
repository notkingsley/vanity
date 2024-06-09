//
// Created by kingsli on 4/29/24.
//

#include "peer_request_server.h"


namespace vanity {

void PeerRequestServer::handle_peer(const std::string &msg, Client &client) {
	try	{
		handle_peer_inner(msg, client);
	}
	catch (const InvalidRequest& e) {
		report_peer(client, report_t::BAD_MESSAGE);
	}
	catch (const Exception& e) {
		logger().error(e.what());
		// TODO: reply with error
	}
	catch (const std::exception& e) {
		logger().error(e.what());
		// TODO: reply with error
	}
	catch (...) {
		logger().error("unknown error in request: " + msg);
		// TODO: reply with error
	}
}

void PeerRequestServer::handle_peer_inner(const std::string &msg, Client &client) {
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

void PeerRequestServer::handle_post_request(PeerRequest& request, Context& ctx) {
	using enum object_t;

	auto op = request.get_op();
	bool end = true;

	switch (op) {
		case peer_op_t::PING: {
			post_request_ping(ctx);
			break;
		}
		case peer_op_t::EXIT: {
			post_request_exit(ctx);
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
			report_peer(ctx.client, report_t::BAD_REQUEST);
		}
	}
}

void PeerRequestServer::handle_reply_request(PeerRequest& request, Context& ctx) {
	using enum object_t;

	auto op = expected_op(ctx.msg_id);
	bool end = true;

	if (not op)
		report_peer(ctx.client, report_t::BAD_REPLY);

	switch (*op) {
		case peer_op_t::PING: {
			reply_request_ping(ctx);
			break;
		}
		case peer_op_t::EXIT: {
			reply_request_exit(ctx);
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
			report_peer(ctx.client, report_t::BAD_REQUEST);
		}
	}
}

} // namespace vanity
