//
// Created by kingsli on 4/19/24.
//

#include "peer_server.h"

namespace vanity {

std::string PeerServer::make_address(const std::string &host, uint16_t port) {
	return host + ":" + std::to_string(port);
}

std::pair<std::string, uint16_t> PeerServer::unmake_address(const std::string &addr) {
	auto pos = addr.rfind(':');
	if (pos == std::string::npos)
		throw std::runtime_error("invalid address");

	auto host = addr.substr(0, pos);
	auto port = std::stoi(addr.substr(pos + 1));
	if (not validate_port(port))
		throw std::runtime_error("invalid port");

	return {host, static_cast<uint16_t>(port)};
}

std::optional<std::pair<std::string, uint16_t>> PeerServer::try_unmake_address(const std::string &addr) {
	try {
		return unmake_address(addr);
	} catch (const std::exception&) {
		return std::nullopt;
	}
}

Client& PeerServer::new_peer(const std::string &host, uint16_t port) {
	auto sock = socket::Socket::connect(host.c_str(), port);
	auto [remote_host, remote_port] = sock.get_remote_addr();
	auto& peer = add_client(TcpClient{std::move(sock)});
	register_peer(peer, make_address(remote_host, remote_port));
	return peer;
}

std::string PeerServer::own_peer_addr() const {
	auto [host, port] = cluster_addr();
	return make_address(host, port);
}

void PeerServer::pre_client_delete_peer(TcpClient &client) {
	forget_peer(client);
}

void PeerServer::forget_peer(TcpClient &client) {
	std::lock_guard lock{m_peers_mutex};
	auto it = m_connected_peers.find(&client);
	if (it == m_connected_peers.end())
		return;

	m_peers.erase(it->second);
	m_connected_peers.erase(it);
}

std::unordered_set<std::string> PeerServer::unknown_peers_in(const std::unordered_set<std::string>& peers) {
	std::unordered_set<std::string> unknown_peers;
	std::lock_guard lock{m_peers_mutex};
	for (const auto& value : peers)
		if (not m_peers.contains(value))
			unknown_peers.insert(value);

	unknown_peers.erase(own_peer_addr());
	return unknown_peers;
}

void PeerServer::register_peer(TcpClient &client, const std::string &addr) {
	std::lock_guard lock{m_peers_mutex};
	m_connected_peers[&client] = addr;
	m_peers.insert(addr);

	session_auth(client) = client_auth::PEER;
	client.session_info().peer_data = std::make_unique<peer_data_t>();
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
	for (auto& [peer, _] : m_connected_peers) {
		post(*peer, peer_op_t::EXIT);
		peer->close();
	}

	m_connected_peers.clear();
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

void PeerServer::request_peers(Client &client) {
	std::lock_guard lock{m_peers_mutex};
	send(client, ok(m_peers));
}

void PeerServer::post_request_peers(Context &ctx) {
	std::lock_guard lock{m_peers_mutex};
	reply(ctx, m_peers);
}

void PeerServer::reply_request_peers(Context&, const std::unordered_set<std::string> &peers) {
	auto& key = get_cluster_key();
	if (not key)
		return;

	std::vector<std::pair<std::string, uint16_t>> unmade_peers;
	auto unknown_peers = unknown_peers_in(peers);
	unmade_peers.reserve(unknown_peers.size());

	for (const auto& value : unknown_peers)
		unmade_peers.emplace_back(unmake_address(value));

	for (const auto& [host, port] : unmade_peers)
		peer_connect(host, port, *key);

	std::lock_guard lock{m_peers_mutex};
	m_peers.merge(unknown_peers);
}

void PeerServer::post_request_exit(Context &ctx) {
	client_close(ctx.client);
}

void PeerServer::reply_request_exit(Context &ctx) {
	throw std::runtime_error("received unexpected reply to exit request");
}

} // namespace vanity
