//
// Created by kingsli on 4/19/24.
//

#ifndef VANITY_PEER_SERVER_H
#define VANITY_PEER_SERVER_H

#include "bind_server.h"
#include "client/client_server.h"
#include "client/exit_server.h"
#include "cluster_auth_server.h"
#include "request/peer_request_server.h"


namespace vanity {

/*
 * A PeerServer holds and manages all peers
 */
class PeerServer:
	public virtual BindServer,
	public virtual ClientServer,
	public virtual ClusterAuthServer,
	public virtual ExitServer,
	public virtual PeerRequestServer
{
private:
	// known peers
	std::unordered_set<std::string> m_peers;

	// connected peers
	std::unordered_map<TcpClient*, std::string> m_connected_peers;

	// mutex for the peers map
	std::mutex m_peers_mutex;

	// make an address by joining a host and a port into a single string
	static std::string make_address(const std::string& host, uint16_t port);

	// unmake an address by splitting a host and a port from a single string
	static std::pair<std::string, uint16_t> unmake_address(const std::string& addr);

	// register a new peer
	void register_peer(TcpClient& client, const std::string& addr);

	// connect to and register a new peer
	Client& new_peer(const std::string& host, uint16_t port);

	// get this server's peer address as a single string
	std::string own_peer_addr() const;

	// the peer hook pre-deleting a client
	// this removes the tcp_client from the peers map
	void pre_client_delete_peer(TcpClient& client) override;

	// remove a peer from the peers set and the connected peers map
	void forget_peer(TcpClient &client);

	// given a set of (potentially unknown) peers, return the set of unknown peers
	// calculates the set difference of m_peers and the peers set
	std::unordered_set<std::string> unknown_peers_in(const std::unordered_set<std::string>& peers);

protected:
	// connect to and authenticate with a peer
	void peer_connect(const std::string& host, uint16_t port, const std::string& key, Client* client = nullptr);

	// sync information with a newly authenticated peer
	// currently, we simply request the peer's peers
	void peer_sync(Client& peer);

public:
	// register a new peer
	void register_peer(Client& client, const std::string& addr);

	// remove a known peer
	void remove_peer(Client& peer);

	// clear all peers
	void clear_peers();

	// a peers request was received from a client
	void request_peers(Client& client) override;

	// a peers request was received from a peer
	void post_request_peers(Context& ctx) override;

	// a reply to a peers request was received from a peer
	void reply_request_peers(Context& ctx, const std::unordered_set<std::string>& peers) override;
};

} // namespace vanity

#endif //VANITY_PEER_SERVER_H
