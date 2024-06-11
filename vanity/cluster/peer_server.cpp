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

std::unordered_set<std::string> PeerServer::unknown_peers_in(const std::unordered_set<std::string>& peers) {
	std::unordered_set<std::string> unknown_peers;
	auto known_peers = peer_addresses();

	for (const auto& value : peers)
		if (not known_peers.contains(value))
			unknown_peers.insert(value);

	unknown_peers.erase(own_peer_addr());
	return unknown_peers;
}

void PeerServer::register_peer(TcpClient &client, const std::string &addr) {
	std::lock_guard lock{m_peers_mutex};
	m_peers.insert(&client);
	session_set_auth(client, client_auth::PEER, addr);
}

void PeerServer::register_peer(Client &client, const std::string &addr) {
	register_peer(to_tcp(client), addr);
}

void PeerServer::remove_peer(Client& client) {
	auto& tcp_client = to_tcp(client);
	forget_peer(tcp_client);
	tcp_client.close();
}

void PeerServer::clear_peers() {
	std::lock_guard lock{m_peers_mutex};
	for (auto& peer: m_peers) {
		post(*peer, peer_op_t::EXIT);
		peer->close();
	}

	m_peers.clear();
}

void PeerServer::peer_connect(const std::string &host, uint16_t port, const std::string &key, Client *client) {
	auto& peer = new_peer(host, port);
	auto id = post_plain(peer, peer_op_t::PEER_AUTH, key, own_peer_addr());
	add_auth_application(id, key, client);
}

void PeerServer::peer_sync(Client &peer) {
	post(peer, peer_op_t::PEERS);
}

std::vector<Client*> PeerServer::get_peers() {
	std::lock_guard lock{m_peers_mutex};

	std::vector<Client*> peers;
	peers.reserve(m_peers.size());
	for (auto& client : m_peers)
		peers.push_back(client);

	return peers;
}

void PeerServer::request_peers(Client &client) {
	send(client, ok(peer_addresses()));
}

void PeerServer::post_request_peers(Context &ctx) {
	reply(ctx, peer_addresses());
}

void PeerServer::reply_request_peers(Context&, const std::unordered_set<std::string> &peers) {
	auto& opt_key = get_cluster_key();
	if (not opt_key)
		return;

	std::vector<std::pair<std::string, uint16_t>> unmade_peers;
	auto unknown_peers = unknown_peers_in(peers);
	unmade_peers.reserve(unknown_peers.size());

	for (const auto& value : unknown_peers)
		unmade_peers.emplace_back(unmake_address(value));

	auto& key = *opt_key;
	for (const auto& [host, port] : unmade_peers)
		peer_connect(host, port, key);
}

void PeerServer::post_request_exit(Context &ctx) {
	client_close(ctx.client);
}

void PeerServer::reply_request_exit(Context &ctx) {
	throw std::runtime_error("received unexpected reply to exit request");
}

} // namespace vanity
