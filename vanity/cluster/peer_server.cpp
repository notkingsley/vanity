//
// Created by kingsli on 4/19/24.
//

#include "peer_server.h"

namespace vanity {

std::unordered_set<std::string> PeerServer::peer_addresses() {
	std::lock_guard lock{m_peers_mutex};

	std::unordered_set<std::string> addresses;
	for (auto& peer: m_peers)
		addresses.insert(session_addr(*peer));

	return addresses;
}

Client& PeerServer::new_peer(const std::string &host, uint16_t port) {
	auto sock = socket::Socket::connect(host.c_str(), port);
	auto [remote_host, remote_port] = sock.get_remote_addr();
	auto& peer = add_client(TcpClient{std::move(sock)});
	register_peer(peer, make_address(remote_host, remote_port));
	return peer;
}

void PeerServer::pre_client_delete_peer(TcpClient &client) {
	forget_peer(client);
}

void PeerServer::forget_peer(TcpClient &client) {
	if (not session_is_peer(client))
		return;

	std::lock_guard lock{m_peers_mutex};
	m_peers.erase(&client);
}

void PeerServer::register_peer(TcpClient &client, const std::string &addr) {
	std::lock_guard lock{m_peers_mutex};
	m_peers.insert(&client);
	session_set_auth(client, client_auth::PEER, addr);
}

void PeerServer::clear_peers() {
	std::lock_guard lock{m_peers_mutex};

	for (auto& peer: m_peers) {
		post(*peer, peer_op_t::EXIT);
		peer->close();
	}

	m_peers.clear();
}

std::vector<Client*> PeerServer::get_peers() {
	std::lock_guard lock{m_peers_mutex};

	std::vector<Client*> peers;
	peers.reserve(m_peers.size());
	for (auto& client : m_peers)
		peers.push_back(client);

	return peers;
}

} // namespace vanity
