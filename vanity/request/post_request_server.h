//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_POST_REQUEST_SERVER_H
#define VANITY_POST_REQUEST_SERVER_H

#include "client/client.h"
#include "post_request.h"
#include "response/reply_message_server.h"


namespace vanity {

/*
 * A PostRequestServer is a server that handles PostRequests
 * from peers
 */
class PostRequestServer : public virtual ReplyMessageServer
{
	// string for a PONG reply
	static constexpr auto PONG = "PONG";

public:
	// handle a PostRequest
	void handle_post_request(PostRequest& request, Client &client);

	// a ping request was received from a peer
	void post_request_ping(Context& ctx);

	// a peer_auth request was received from a peer
	virtual void post_request_peer_auth(Context& ctx, const std::string& key, const std::string& addr) = 0;
};

} // namespace vanity

#endif //VANITY_POST_REQUEST_SERVER_H
