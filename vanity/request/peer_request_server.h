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
	// handle an ASYNC request
	void handle_async_request(PeerRequest& peer_request, Client& client);

	// handle a POST request
	void handle_post_request(PeerRequest& peer_request, Context& ctx);

	// handle a REPLY request
	void handle_reply_request(PeerRequest& peer_request, Context& ctx);

	// handle an OK REPLY request
	void handle_ok_reply_request(PeerRequest& peer_request, Context& ctx, peer_op_t op);

	// handle an ERR REPLY request
	void handle_err_reply_request(PeerRequest& peer_request, Context& ctx, peer_op_t op);

	// handle a DENIED REPLY request
	void handle_denied_reply_request(PeerRequest& peer_request, Context& ctx, peer_op_t op);

	// handle a REDIRECT REPLY request
	void handle_redirect_reply_request(PeerRequest& peer_request, Context& ctx, peer_op_t op);

	// get a peer's string for error logging
	static std::string get_peer_str(Client& client);

	// default handler for ERR REPLY requests
	void handle_default_err_reply_request(PeerRequest& peer_request, Context& ctx, peer_op_t op);

	// default handler for DENIED REPLY requests
	void handle_default_denied_reply_request(PeerRequest& peer_request, Context& ctx, peer_op_t op);

	// default handler for REDIRECT REPLY requests
	void handle_default_redirect_reply_request(PeerRequest& peer_request, Context& ctx, peer_op_t op);

	// same as handle_peer, but doesn't handle exceptions
	void handle_peer_inner(const std::string &msg, Client &client);

protected:
	// handle a peer's request
	void handle_peer(const std::string &msg, Client &client);

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
	virtual void post_request_peer_auth(Context& ctx) = 0;

	// a reply to a peer_auth request was received from a peer
	virtual void reply_request_peer_auth(Context& ctx, const std::string& data) = 0;

	// a DENIED reply to a peer_auth request was received from a peer
	virtual void reply_denied_request_peer_auth(Context& ctx) = 0;

	// a REDIRECT reply to a peer_auth request was received from a peer
	virtual void reply_redirect_request_peer_auth(Context& ctx, const std::string& addr) = 0;

	// a peers request was received from a peer
	virtual void post_request_peers(Context& ctx) = 0;

	// a reply to a peers request was received from a peer
	virtual void reply_request_peers(Context& ctx, const std::unordered_set<std::string>& peers) = 0;

	// an ask_evict request was received from a peer
	virtual void post_request_ask_evict(Context& ctx, const std::string& issue_id, const std::string& peer_addr, const std::string& reason) = 0;

	// a reply to an ask_evict request was received from a peer
	virtual void reply_request_ask_evict(Context& ctx, bool opinion) = 0;

	// a pulse async request was received from a peer
	virtual void async_request_pulse(Client& client) = 0;
};

} // namespace vanity

#endif //VANITY_PEER_REQUEST_SERVER_H
