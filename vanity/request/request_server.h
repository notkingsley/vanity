//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_REQUEST_SERVER_H
#define VANITY_REQUEST_SERVER_H

#include <list>
#include <unordered_set>

#include "abstract_server.h"
#include "log_server.h"
#include "request_handler.h"
#include "response/response_server.h"
#include "session_server.h"


namespace vanity {

/*
 * A RequestServer allows to process incoming requests
 * and dispatch them to the appropriate handler
 */
class RequestServer :
	public virtual AbstractServer,
	public virtual RequestHandler,
	public virtual ResponseServer,
	public virtual LogServer,
	public virtual SessionServer
{
public:
	// a ping request was received from a client
	void request_ping(Client& client) override {
		send(client, pong());
	};

protected:
	// a message was received from a user client
	void handle_user(const std::string& msg, Client& client);

	// dispatch multiple requests using do_handle, expecting the message to end after the last one
	// this calls do_handle_single on each request in the message,
	// so a response will be sent for each one no matter what
	// behavior is undefined if one of the requests couldn't be dispatched
	void do_handle(Client& client, Request& request, size_t len);

	// extract the data from a request and dispatch it to the appropriate handler
	// client: the client that sent the message
	// request: the request to extract data from
	// end: whether the message should end after this operation. an error is sent if it doesn't
	// return true if the request was extracted and dispatched successfully, false otherwise
	bool do_handle_single(Client& client, Request& request, bool end);

	// dispatch a request based on the client's behavior
	// this selects the correct handler based on the client's current state
	// effectively same as do_handle, but doesn't catch errors or guarantee a response will be sent
	bool dispatch_by_behavior(Client& client, Request& request, bool end);

	// dispatch a request in the default behavior
	// this is the default behavior for a client, and is used when the client is not in any special state
	// or when the request is not affected by the client's state/behavior
	// returns true if the request was dispatched successfully, false otherwise
	bool dispatch_default_behavior(Client& client, Request& request, bool end);

	// dispatch a request not permitted in the current state
	// this is called when a request is not permitted in the current state
	// and is distinct from the request not being permitted due to the client's auth level
	// returns true if the request was dispatched successfully, false otherwise
	bool dispatch_not_permitted_behavior(Client& client, Request& request, bool end);

	// dispatch a request in a contextual behavior
	// this is called when a request is permitted in the current state, but is affected by the client's state/behavior
	// returns true if the request was dispatched successfully, false otherwise
	bool dispatch_contextual_behavior(Client& client, Request& request, bool end);

	// dispatch a request in a normal context
	bool dispatch_normal_context(Client& client, Request& request, bool end);

	// dispatch a request in a transaction context
	virtual bool dispatch_transaction_context(Client& client, Request& request, bool end) = 0;

	// send a response to the client, calling dry_dispatch if necessary
	// always returns false
	bool refuse_with_response(Client& client, Request& request, Response&& response, bool end);
};

} // namespace vanity

#endif //VANITY_REQUEST_SERVER_H
