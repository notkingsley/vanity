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
	PeerRequest request {msg};

	switch (request.get_request_t()) {
		case peer_request_t::ASYNC: {
			handle_async_request(request, client);
			break;
		}

		case peer_request_t::POST: {
			Context ctx {request.get<object_t::INT>(), client};
			handle_post_request(request, ctx);
			break;
		}

		case peer_request_t::REPLY: {
			Context ctx {request.get<object_t::INT>(), client};
			handle_reply_request(request, ctx);
			break;
		}
	}
}

void PeerRequestServer::handle_async_request(PeerRequest& request, Client& client) {
	using enum object_t;

	auto op = request.get_async_op();
	bool end = true;

	switch (op) {
		case async_op_t::PULSE: {
			request.get_exact<>(end);
			async_request_pulse(client);
			break;
		}

		case async_op_t::MAX_OP: {
			request.get_exact<>(end);
			report_peer(client, report_t::BAD_REQUEST);
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
			request.get_exact<>(end);
			post_request_ping(ctx);
			break;
		}
		case peer_op_t::EXIT: {
			post_request_exit(ctx);
			request.get_exact<>(end);
			break;
		}

		case peer_op_t::PEER_AUTH: {
			request.get_exact<>(end);
			post_request_peer_auth(ctx);
			break;
		}
		case peer_op_t::PEERS: {
			request.get_exact<>(end);
			post_request_peers(ctx);
			break;
		}

		case peer_op_t::ASK_EVICT: {
			auto [issue_id, peer_addr, reason] = request.get_exact<STR, STR, STR>(end);
			post_request_ask_evict(ctx, issue_id, peer_addr, reason);
			break;
		}

		case peer_op_t::MAX_OP: {
			report_peer(ctx.client, report_t::BAD_REQUEST);
			request.get_exact<>(end);
			break;
		}
	}
}

void PeerRequestServer::handle_reply_request(PeerRequest& request, Context& ctx) {
	auto op = expected_op(ctx.msg_id);
	if (not op)
		return report_peer(ctx.client, report_t::BAD_REPLY);

	switch (request.get_reply_status()) {
		case ReplyStatus::OK:
			handle_ok_reply_request(request, ctx, *op);
			break;
		case ReplyStatus::ERR:
			handle_err_reply_request(request, ctx, *op);
			break;
		case ReplyStatus::DENIED:
			handle_denied_reply_request(request, ctx, *op);
			break;
		case ReplyStatus::REDIRECT:
			handle_redirect_reply_request(request, ctx, *op);
			break;
		case ReplyStatus::MAX_STATUS:
			report_peer(ctx.client, report_t::BAD_REPLY);
			break;
	}
}

void PeerRequestServer::handle_ok_reply_request(PeerRequest& request, Context& ctx, peer_op_t op) {
	using enum object_t;

	bool end = true;
	switch (op) {
		case peer_op_t::PING: {
			auto pong = request.get_exact<STR>(end);
			reply_request_ping(ctx);
			break;
		}
		case peer_op_t::EXIT: {
			request.get_exact<>(end);
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

		case peer_op_t::ASK_EVICT: {
			auto opinion = request.get_exact<BOOL>(end);
			reply_request_ask_evict(ctx, opinion);
			break;
		}

		case peer_op_t::MAX_OP: {
			report_peer(ctx.client, report_t::BAD_REQUEST);
			request.get_exact<>(end);
			break;
		}
	}
}

void PeerRequestServer::handle_err_reply_request(PeerRequest& request, Context& ctx, peer_op_t op) {
	bool end = true;
	switch (op) {
		case peer_op_t::PING:
		case peer_op_t::EXIT:

		case peer_op_t::PEER_AUTH:
		case peer_op_t::PEERS:

		case peer_op_t::ASK_EVICT:
			handle_default_err_reply_request(request, ctx, op);
			break;

		case peer_op_t::MAX_OP: {
			report_peer(ctx.client, report_t::BAD_REQUEST);
			request.get_exact<>(end);
			break;
		}
	}
}

void PeerRequestServer::handle_denied_reply_request(PeerRequest& request, Context& ctx, peer_op_t op) {
	bool end = true;
	switch (op) {
		case peer_op_t::PEER_AUTH: {
			request.get_exact<>(end);
			reply_denied_request_peer_auth(ctx);
			break;
		}


		case peer_op_t::PING:
		case peer_op_t::EXIT:

		case peer_op_t::PEERS:

		case peer_op_t::ASK_EVICT:
			handle_default_denied_reply_request(request, ctx, op);
			break;

		case peer_op_t::MAX_OP: {
			report_peer(ctx.client, report_t::BAD_REQUEST);
			request.get_exact<>(end);
			break;
		}
	}
}

void PeerRequestServer::handle_redirect_reply_request(PeerRequest& request, Context& ctx, peer_op_t op) {
	bool end = true;
	switch (op) {
		case peer_op_t::PEER_AUTH: {
			auto addr = request.get_exact<object_t::STR>(end);
			reply_redirect_request_peer_auth(ctx, addr);
			break;
		}


		case peer_op_t::PING:
		case peer_op_t::EXIT:

		case peer_op_t::PEERS:

		case peer_op_t::ASK_EVICT:
			handle_default_redirect_reply_request(request, ctx, op);
			break;

		case peer_op_t::MAX_OP: {
			report_peer(ctx.client, report_t::BAD_REQUEST);
			request.get_exact<>(end);
			break;
		}
	}
}

std::string PeerRequestServer::get_peer_str(Client& client) {
	if (auto& peer_id = session_id(client))
		return " from peer with id: " + *peer_id;
	else
		return " from unidentified peer with address: " + session_addr(client);
}

void PeerRequestServer::handle_default_err_reply_request(PeerRequest& request, Context& ctx, peer_op_t op) {
	auto peer = get_peer_str(ctx.client);
	auto err = request.get_exact<object_t::STR>(true);
	logger().error("unexpected error reply: " + err + " for op: " + PostMessage::op_to_string(op) + peer);
}

void PeerRequestServer::handle_default_denied_reply_request(PeerRequest& request, Context& ctx, peer_op_t op) {
	using namespace std::string_literals;

	request.get_exact<>(true);
	auto peer = get_peer_str(ctx.client);
	logger().error("unexpected denied reply for op: "s + PostMessage::op_to_string(op) + peer);
}

void PeerRequestServer::handle_default_redirect_reply_request(PeerRequest& request, Context& ctx, peer_op_t op) {
	using namespace std::string_literals;

	request.get_exact<>(true);
	auto peer = get_peer_str(ctx.client);
	logger().error("unexpected redirect reply for op: "s + PostMessage::op_to_string(op) + peer);
}

} // namespace vanity
