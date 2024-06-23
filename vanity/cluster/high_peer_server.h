//
// Created by kingsli on 6/11/24.
//

#ifndef VANITY_HIGH_PEER_SERVER_H
#define VANITY_HIGH_PEER_SERVER_H

#include "client/exit_server.h"
#include "peer_server.h"
#include "request/peer_request_server.h"


namespace vanity {

/*
 * A bit lazy, but a HighPeerServer is really just
 * a PeerServer with higher level functionalities
 * and that handles requests
 */
class HighPeerServer:
	public virtual ExitServer,
	public virtual PeerRequestServer,
	public virtual PeerServer
{
private:
	// given a set of (potentially unknown) peers, return the set of unknown peers
	// calculates the set difference of m_peers and the peers set
	std::unordered_set<std::string> unknown_peers_in(const std::unordered_set<std::string>& peers);

protected:
	// connect to and authenticate with a peer
	void peer_connect(const std::string& host, uint16_t port, const std::string& key, const std::string& peer_id, Client* client = nullptr);

	// sync information with a newly authenticated peer
	// currently, we simply request the peer's peers
	void peer_sync(Client& peer);

	// register a new peer
	void register_peer(Client& client, const std::string& addr, const std::string& peer_id);

	// remove a known peer
	void remove_peer(Client& peer);

public:
	// a peers request was received from a client
	void request_peers(Client& client) override;

	// a peers request was received from a peer
	void post_request_peers(Context& ctx) override;

	// a reply to a peers request was received from a peer
	void reply_request_peers(Context& ctx, const std::unordered_set<std::string>& peers) override;

	// an exit request was received from a peer
	void post_request_exit(Context& ctx) override;

	// a reply to an exit request was received from a peer
	void reply_request_exit(Context& ctx) override;
};

} // namespace vanity

#endif //VANITY_HIGH_PEER_SERVER_H
