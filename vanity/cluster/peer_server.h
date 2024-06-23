//
// Created by kingsli on 4/19/24.
//

#ifndef VANITY_PEER_SERVER_H
#define VANITY_PEER_SERVER_H

#include "address_server.h"
#include "client/client_server.h"
#include "cluster_auth_server.h"
#include "response/peer_message_server.h"


namespace vanity {

/*
 * A PeerServer holds and manages all peers
 */
class PeerServer:
	public virtual AddressServer,
	public virtual ClientServer,
	public virtual ClusterAuthServer,
	public virtual PeerMessageServer
{
private:
	// known peers
	std::unordered_set<TcpClient*> m_peers;

	// mutex for the peers map
	std::mutex m_peers_mutex;

	// the peer hook pre-deleting a client
	// this removes the tcp_client from the peers map
	void pre_client_delete_peer(TcpClient& client) override;

protected:
	// return a set of all peers' addresses
	std::unordered_set<std::string> peer_addresses();

	// return a set of all peers' ids
	std::unordered_set<std::string> peer_ids();

	// register a new peer
	void register_peer(TcpClient& client, const std::string& addr, std::optional<std::string> peer_id);

	// connect to and register a new peer
	Client& new_peer(const std::string& host, uint16_t port);

	// remove a peer from the peers set and the connected peers map
	void forget_peer(TcpClient &client);

	// get all active peers
	std::vector<Client*> get_peers();

	// clear all peers
	void clear_peers();
};

} // namespace vanity

#endif //VANITY_PEER_SERVER_H
