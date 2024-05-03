//
// Created by kingsli on 5/3/24.
//

#ifndef VANITY_REPLY_REQUEST_SERVER_H
#define VANITY_REPLY_REQUEST_SERVER_H

#include "reply_request.h"
#include "response/post_message_server.h"
#include "response/reply_message_server.h"

namespace vanity {

/*
 * A ReplyRequestServer is a server that handles replies from peers
 */
class ReplyRequestServer : public virtual PostMessageServer, public virtual ReplyMessageServer
{
public:
	// handle a peer's reply
	void handle_reply_request(ReplyRequest& request, Client &client);

	// a reply to a ping request was received from a peer
	void reply_request_ping(Client &client);
};

} // namespace vanity

#endif //VANITY_REPLY_REQUEST_SERVER_H
