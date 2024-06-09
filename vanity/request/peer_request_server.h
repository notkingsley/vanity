//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_PEER_REQUEST_SERVER_H
#define VANITY_PEER_REQUEST_SERVER_H

#include "cluster/peer_behaviour_server.h"
#include "peer_request.h"
#include "log_server.h"
#include "response/peer_message_server.h"


namespace vanity {

/*
 * A PeerRequestServer is a server that handles messages from peers
 */
class PeerRequestServer:
	public virtual LogServer,
	public virtual PeerBehaviourServer,
	public virtual PeerMessageServer
{
private:
	// handle a POST request
	void handle_post_request(PeerRequest& peer_request, Context& ctx);

	// handle a REPLY request
	void handle_reply_request(PeerRequest& peer_request, Context& ctx);

protected:
	// handle a peer's request
	void handle_peer(const std::string &msg, Client &client);

	// same as handle_peer, but doesn't handle exceptions
	void handle_peer_inner(const std::string &msg, Client &client);

public:
	// a ping request was received from a peer
	virtual void post_request_ping(Context& ctx) { reply(ctx, "PONG"); }

	// a reply to a ping request was received from a peer
	virtual void reply_request_ping(Context& ctx) { }

	// an exit request was received from a peer
	virtual void post_request_exit(Context& ctx) = 0;

	// a reply to an exit request was received from a peer
	virtual void reply_request_exit(Context& ctx) = 0;

	// a peer_auth request was received from a peer
	virtual void post_request_peer_auth(Context& ctx, const std::string& key, const std::string& addr) = 0;

	// a reply to a peer_auth request was received from a peer
	virtual void reply_request_peer_auth(Context& ctx, const std::string& data) = 0;

	// a peers request was received from a peer
	virtual void post_request_peers(Context& ctx) = 0;

	// a reply to a peers request was received from a peer
	virtual void reply_request_peers(Context& ctx, const std::unordered_set<std::string>& peers) = 0;
};

} // namespace vanity

#endif //VANITY_PEER_REQUEST_SERVER_H
