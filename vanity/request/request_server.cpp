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
		return dispatch_or_refuse(client, request, end);
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

bool RequestServer::dispatch_or_refuse(Client &client, Request& request, bool end) {
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
