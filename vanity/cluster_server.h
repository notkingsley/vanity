//
// Created by kingsli on 4/16/24.
//

#ifndef VANITY_CLUSTER_SERVER_H
#define VANITY_CLUSTER_SERVER_H

#include "bind_server.h"
#include "peer_server.h"
#include "request/peer_request_server.h"
#include "utils/hash.h"


namespace vanity {

/*
 * A ClusterServer connects to other servers in a cluster
 */
class ClusterServer : public virtual BindServer, public virtual PeerRequestServer, public virtual PeerServer
{
private:
	static constexpr auto M_MIN_CLUSTER_KEY_LEN = 12;

	// the key of the cluster, if any
	std::optional<std::string> m_cluster_key;

	// return a PEER_AUTH response
	static Response peer_auth(const std::string& key, const std::string& addr);

	// make an address by joining a host and a port into a string
	static std::string make_address(const std::string& host, uint16_t port);

	// validate a cluster key to authenticate a peer
	// returns true if the key is valid, false otherwise
	bool validate_cluster_key(const std::string& key);

	// get this server's address
	std::string get_own_address() const;

	// connect to a remote server
	TcpClient& connect(const std::string &host, uint16_t port);

public:
	// a message was received from a client
	void handle(const std::string& msg, Client& client) override;

	// a message was received from a peer client
	void handle_peer_tmp(const std::string& msg, Client& client);

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
};

} // namespace vanity

#endif //VANITY_CLUSTER_SERVER_H
