//
// Created by kingsli on 4/19/24.
//

#ifndef VANITY_PEER_SERVER_H
#define VANITY_PEER_SERVER_H

#include "bind_server.h"
#include "client/client_server.h"
#include "client/exit_server.h"
#include "request/peer_request_server.h"


namespace vanity {

/*
 * A PeerServer holds and manages all peers
 */
class PeerServer:
	public virtual BindServer,
	public virtual ClientServer,
	public virtual ExitServer,
	public virtual PeerRequestServer
{
private:
	// known peers
	std::unordered_map<TcpClient*, std::string> m_peers;

	// mutex for the peers map
	std::mutex m_peers_mutex;

	// make an address by joining a host and a port into a single string
	static std::string make_address(const std::string& host, uint16_t port);

	// the peer hook pre-deleting a client
	// this removes the tcp_client from the peers map
	void pre_client_delete_peer(TcpClient& client) override;

	// connect to a remote server
	TcpClient& connect(const std::string &host, uint16_t port);

	// register a new peer
	void register_peer(TcpClient& client, const std::string& addr);

protected:
	// connect to and register a new peer
	Client& peer_connect(const std::string& host, uint16_t port);

	// get this server's peer address as a single string
	std::string own_peer_addr() const;

	// get a set of all known peers
	std::unordered_set<std::string> get_peers();

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
