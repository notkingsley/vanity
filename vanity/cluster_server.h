//
// Created by kingsli on 4/16/24.
//

#ifndef VANITY_CLUSTER_SERVER_H
#define VANITY_CLUSTER_SERVER_H

#include "client/client_server.h"
#include "request/request_server.h"
#include "utils/hash.h"


namespace vanity {

/*
 * A ClusterServer connects to other servers in a cluster
 */
class ClusterServer : public virtual ClientServer, public virtual RequestServer
{
private:
	static constexpr auto M_MIN_CLUSTER_KEY_LEN = 12;

	// known peers
	std::unordered_map<
	    std::pair<std::string, uint16_t>,
		TcpClient*,
		boost_hash::hash<std::string, uint16_t>
	> m_peers;

	// the key of the cluster, if any
	std::optional<std::string> m_cluster_key;

public:
	// a message was received from a client
	void handle(const std::string& msg, Client& client) override;

	// a message was received from a peer client
	void handle_peer(const std::string& msg, Client& client);

	// connect to a remote server
	TcpClient& connect(const std::string& host, uint16_t port);

	// a cluster_join request was received from a client
	void request_cluster_join(Client& client, const std::string& host, uint16_t port) override;

	// a cluster_leave request was received from a client
	void request_cluster_leave(Client& client) override;

	// a cluster_key request was received from a client
	void request_cluster_key(Client& client) override;

	// a cluster_new request was received from a client
	void request_cluster_new(Client& client, const std::string& key) override;

	// a peers request was received from a client
	void request_peers(Client& client) override;

	// a peer_auth request was received from a client
	void request_peer_auth(Client& client, const std::string& key) override;
};

} // namespace vanity

#endif //VANITY_CLUSTER_SERVER_H
