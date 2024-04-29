//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_PEER_REQUEST_SERVER_H
#define VANITY_PEER_REQUEST_SERVER_H

#include "peer_request.h"
#include "post_request_server.h"

namespace vanity {

/*
 * A PeerRequestServer is a server that handles messages from peers
 */
class PeerRequestServer : public virtual PostRequestServer
{
public:
	// handle a PeerRequest
	void handle_peer_request(PeerRequest& request, Client &client);

	void handle_peer(const std::string &msg, Client &client);
};

} // namespace vanity

#endif //VANITY_PEER_REQUEST_SERVER_H
