//
// Created by kingsli on 4/19/24.
//

#include "peer_server.h"

namespace vanity {

void PeerServer::pre_client_delete_peer(TcpClient &client) {
	m_peers.erase(&client);
}

void PeerServer::register_peer(TcpClient &client, const std::string &addr) {
	m_peers[&client] = addr;
	session_auth(client) = client_auth::PEER;
}

void PeerServer::register_peer(Client &client, const std::string &addr) {
	register_peer(to_tcp(client), addr);
}

void PeerServer::remove_peer(Client& client) {
	auto& tcp_client = to_tcp(client);
	m_peers.erase(&tcp_client);
	tcp_client.close();
}

void PeerServer::clear_peers() {
	for (auto& [peer, _] : m_peers)
		peer->close();

	m_peers.clear();
}

void PeerServer::request_peers(Client &client) {
	std::vector<std::string> peers;
	peers.reserve(m_peers.size());

	for (auto& [_, peer_addr] : m_peers)
		peers.push_back(peer_addr);

	send(client, ok(peers));
}

} // namespace vanity
