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

private:
	// extract the data from a request and dispatch it to the appropriate handler
	// client: the client that sent the message
	// request: the request to extract data from
	// end: whether the message should end after this operation. an error is sent if it doesn't
	// return true if the request was extracted and dispatched successfully, false otherwise
	bool do_handle_single(Client& client, Request& request, bool end);

	// check for permissions and dispatch a request or refuse it
	bool dispatch_or_refuse(Client& client, Request& request, bool end);

	// send a response to the client, calling dry_dispatch if necessary
	// always returns false
	bool refuse_with_response(Client& client, Request& request, Response&& response, bool end);
};

} // namespace vanity

#endif //VANITY_REQUEST_SERVER_H
