//
// Created by kingsli on 9/14/23.
//

#include "request_server.h"
#include "request_tracker.h"

namespace vanity {

void RequestServer::handle_user(const std::string& msg, Client& client) {
	Request request{msg};
	do_handle(client, request, 1);
}

void RequestServer::do_handle(Client &client, Request &request, size_t len) {
	if (len == 0)
		return;

	for (size_t i = 0; i < len - 1; ++i)
		do_handle_single(client, request, false);
	do_handle_single(client, request, true);
}

bool RequestServer::do_handle_single(Client &client, Request& request, bool end) {
	try
	{
		return dispatch_by_behavior(client, request, end);
	}
	catch (const InvalidRequest& e)
	{
		send(client, bad_request(e.what()));
		return false;
	}
	catch (const Exception& e)
	{
		logger().error(e.what());
		send(client, internal_error(e.what()));
		return true;
	}
	catch (const std::exception& e)
	{
		logger().error(e.what());
		send(client, internal_error(e.what()));
		return true;
	}
	catch (...)
	{
		logger().error("unknown error in request: " + request.format());
		send(client, internal_error("unknown error"));
		return true;
	}
}

bool RequestServer::dispatch_by_behavior(Client &client, Request& request, bool end) {
	switch (behaviour(request.peek_operation(), session_state(client))) {
		case behaviour_t::DEFAULT:
		{
			return dispatch_default_behavior(client, request, end);
		}
		case behaviour_t::NOT_PERMITTED:
		{
			return dispatch_not_permitted_behavior(client, request, end);
		}
		case behaviour_t::CONTEXTUAL:
		{
			return dispatch_contextual_behavior(client, request, end);
		}
		default:
		{
			throw std::runtime_error("invalid behaviour");
		}
	}
}

bool RequestServer::dispatch_default_behavior(Client& client, Request& request, bool end) {
	return dispatch_normal_context(client, request, end);
}

bool RequestServer::dispatch_not_permitted_behavior(Client& client, Request& request, bool end) {
	return refuse_with_response(client, request, bad_state(), end);
}

bool RequestServer::dispatch_contextual_behavior(Client& client, Request& request, bool end) {
	switch (session_state(client)) {
		case user_data_t::	conn_state::NORMAL:
		{
			return dispatch_normal_context(client, request, end);
		}
		case user_data_t::conn_state::TRANSACTION:
		{
			return dispatch_transaction_context(client, request, end);
		}
		default:
		{
			throw std::runtime_error("invalid state");
		}
	}
}

bool RequestServer::dispatch_normal_context(Client &client, Request& request, bool end) {
	if (not client.has_perm(request.peek_operation()))
		return refuse_with_response(client, request, denied(), end);

	dispatch(client, request, end);
	return true;
}

bool RequestServer::refuse_with_response(Client& client, Request& request, Response&& response, bool end) {
	if (not end)
		dry_dispatch(request, end);

	send(client, response.move());
	return false;
}

} // namespace vanity
