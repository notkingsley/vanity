//
// Created by kingsli on 4/16/24.
//

#ifndef VANITY_CLUSTER_SERVER_H
#define VANITY_CLUSTER_SERVER_H

#include "bind_server.h"
#include "client/client_server.h"
#include "request/request_server.h"
#include "utils/hash.h"


namespace vanity {

/*
 * A ClusterServer connects to other servers in a cluster
 */
class ClusterServer:
	public virtual BindServer,
	public virtual ClientServer,
	public virtual RequestServer
{
private:
	static constexpr auto M_MIN_CLUSTER_KEY_LEN = 12;

	// known peers
	std::unordered_map<TcpClient*, std::string> m_peers;

	// the key of the cluster, if any
	std::optional<std::string> m_cluster_key;

	// return a PEER_AUTH response
	static Response peer_auth(const std::string& key, const std::string& addr);

	// join a host and a port into a string
	static std::string join_host_port(const std::string& host, uint16_t port);

	// cast a client to a TcpClient or throw an exception
	static TcpClient& to_tcp(Client& client);

	// validate a cluster key to authenticate a peer
	// returns true if the key is valid, false otherwise
	bool validate_cluster_key(const std::string& key);

	// get this server's address
	std::string get_own_address() const;

	// connect to a remote server
	TcpClient& connect(const char* host, uint16_t port);

	// connect to a new peer
	TcpClient& new_peer(const std::string& host, uint16_t port);

	// remove a known peer
	void remove_peer(Client& peer);

public:
	// a message was received from a client
	void handle(const std::string& msg, Client& client) override;

	// a message was received from a peer client
	void handle_peer(const std::string& msg, Client& client);

	// a cluster_join request was received from a client
	void request_cluster_join(Client& client, const std::string& key, const std::string& host, uint16_t port) override;

	// a cluster_leave request was received from a client
	void request_cluster_leave(Client& client) override;

	// a cluster_key request was received from a client
	void request_cluster_key(Client& client) override;

	// a cluster_new request was received from a client
	void request_cluster_new(Client& client, const std::string& key) override;

	// a peers request was received from a client
	void request_peers(Client& client) override;

	// a peer_auth request was received from a client
	void request_peer_auth(Client& client, const std::string& key, const std::string& addr) override;
};

} // namespace vanity

#endif //VANITY_CLUSTER_SERVER_H
