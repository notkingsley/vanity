//
// Created by kingsli on 4/16/24.
//

#ifndef VANITY_CLUSTER_SERVER_H
#define VANITY_CLUSTER_SERVER_H

#include "peer_server.h"
#include "request/peer_request_server.h"
#include "utils/hash.h"


namespace vanity {

/*
 * A ClusterServer connects to other servers in a cluster
 */
class ClusterServer : public virtual PeerRequestServer, public virtual PeerServer
{
private:
	// a pending application we've sent as a peer_auth request
	struct PendingPeerAuth {
		// the key of the application
		std::string key;

		// the client that sent the request
		Client& client;
	};

	// the minimum length of a cluster key
	static constexpr auto M_MIN_CLUSTER_KEY_LEN = 12;

	// all pending peer_auth requests
	std::unordered_map<msg_id_t, PendingPeerAuth> m_pending_peer_auths;

	// the mutex for the pending peer_auth requests
	std::mutex m_pending_peer_auths_mutex;

	// the key of the cluster, if any
	std::optional<std::string> m_cluster_key;

	// the mutex for the cluster key
	std::mutex m_cluster_key_mutex;

	// validate a cluster key to authenticate a peer
	// returns true if the key is valid, false otherwise
	bool validate_cluster_key(const std::string& key);

	// sets the cluster key
	void set_cluster_key(const std::string& key);

	// add a peer_auth request to the pending requests
	void add_auth_application(msg_id_t id, const std::string& key, Client& client);

	// pop a pending peer_auth request
	std::optional<PendingPeerAuth> pop_auth_application(msg_id_t id);

	// clear all pending peer_auth requests
	void clear_auth_applications();

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
	void request_cluster_new(Client& client, const std::string& key) override;

	// a peer_auth request was received from a client
	void request_peer_auth(Client& client, int64_t id, const std::string& key, const std::string& addr) override;

	// a peer_auth request was received from a peer
	void post_request_peer_auth(Context& ctx, const std::string& key, const std::string& addr) override;

	// a reply to a peer_auth request was received from a peer
	void reply_request_peer_auth(Context& ctx, const std::string& data) override;
};

} // namespace vanity

#endif //VANITY_CLUSTER_SERVER_H
