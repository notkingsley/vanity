//
// Created by kingsli on 4/19/24.
//

#ifndef VANITY_PEER_SERVER_H
#define VANITY_PEER_SERVER_H

#include "bind_server.h"
#include "client/client_server.h"
#include "client/exit_server.h"


namespace vanity {

/*
 * A PeerServer holds and manages all peers
 */
class PeerServer : public virtual BindServer, public virtual ClientServer, public virtual ExitServer
{
private:
	// known peers
	std::unordered_map<TcpClient*, std::string> m_peers;

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

public:
	// register a new peer
	void register_peer(Client& client, const std::string& addr);

	// remove a known peer
	void remove_peer(Client& peer);

	// clear all peers
	void clear_peers();

	// a peers request was received from a client
	void request_peers(Client& client) override;
};

} // namespace vanity

#endif //VANITY_PEER_SERVER_H
