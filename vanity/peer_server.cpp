//
// Created by kingsli on 4/19/24.
//

#include "peer_server.h"

namespace vanity {

void PeerServer::pre_client_delete_peer(TcpClient &client) {
	std::lock_guard lock{m_peers_mutex};
	m_peers.erase(&client);
}

std::string PeerServer::make_address(const std::string &host, uint16_t port) {
	return host + ":" + std::to_string(port);
}

TcpClient &PeerServer::connect(const std::string &host, uint16_t port) {
	return add_client(TcpClient{socket::Socket::connect(host.c_str(), port)});
}

Client& PeerServer::peer_connect(const std::string &host, uint16_t port) {
	auto& peer = connect(host, port);
	register_peer(peer, make_address(host, port));
	return peer;
}

std::string PeerServer::own_peer_addr() const {
	auto [host, port] = cluster_addr();
	return make_address(host, port);
}

void PeerServer::register_peer(TcpClient &client, const std::string &addr) {
	std::lock_guard lock{m_peers_mutex};
	m_peers[&client] = addr;
	session_auth(client) = client_auth::PEER;
}

void PeerServer::register_peer(Client &client, const std::string &addr) {
	register_peer(to_tcp(client), addr);
}

void PeerServer::remove_peer(Client& client) {
	std::lock_guard lock{m_peers_mutex};
	auto& tcp_client = to_tcp(client);
	m_peers.erase(&tcp_client);
	tcp_client.close();
}

void PeerServer::clear_peers() {
	std::lock_guard lock{m_peers_mutex};
	for (auto& [peer, _] : m_peers)
		peer->close();

	m_peers.clear();
}

void PeerServer::request_peers(Client &client) {
	send(client, ok(get_peers()));
}

std::unordered_set<std::string> PeerServer::get_peers() {
	std::lock_guard lock{m_peers_mutex};
	std::unordered_set<std::string> peers;
	peers.reserve(m_peers.size());
	for (auto& [_, peer_addr] : m_peers)
		peers.insert(peer_addr);

	return peers;
}

} // namespace vanity
