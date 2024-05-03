//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_POST_REQUEST_SERVER_H
#define VANITY_POST_REQUEST_SERVER_H

#include "client/client.h"
#include "post_request.h"

namespace vanity {

/*
 * A PostRequestServer is a server that handles PostRequests
 * from peers
 */
class PostRequestServer
{
protected:
	// context passed to handlers
	struct Context {
		msg_id_t msg_id;
		Client &client;
	};

public:
	// handle a PostRequest
	void handle_post_request(PostRequest& request, Client &client);

	// a ping request was received from a peer
	void post_request_ping(Context& ctx);
};

} // namespace vanity

#endif //VANITY_POST_REQUEST_SERVER_H
