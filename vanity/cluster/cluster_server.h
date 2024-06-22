//
// Created by kingsli on 4/16/24.
//

#ifndef VANITY_CLUSTER_SERVER_H
#define VANITY_CLUSTER_SERVER_H

#include "high_peer_server.h"

namespace vanity {

/*
 * A ClusterServer connects to other servers in a cluster
 */
class ClusterServer : public virtual HighPeerServer
{
private:
	// check is a client is connected via the cluster port
	bool is_on_cluster_port(Client& client) const;

public:
	// a message was received from a client
	void handle(const std::string& msg, Client& client) override;

	// a cluster_join request was received from a client
	void request_cluster_join(Client& client, const std::string& key, const std::string& host, uint16_t port) override;

	// a cluster_leave request was received from a client
	void request_cluster_leave(Client& client) override;

	// a cluster_key request was received from a client
	void request_cluster_key(Client& client) override;

	// a cluster_new request was received from a client
	void request_cluster_new(Client& client, const std::string& key, const std::string& id) override;

	// a peer_auth request was received from a client
	void request_peer_auth(Client& client, int64_t id, const std::string& key, const std::string& addr) override;

	// a peer_auth request was received from a peer
	void post_request_peer_auth(Context& ctx, const std::string& key, const std::string& addr) override;

	// a reply to a peer_auth request was received from a peer
	void reply_request_peer_auth(Context& ctx, const std::string& data) override;
};

} // namespace vanity

#endif //VANITY_CLUSTER_SERVER_H
