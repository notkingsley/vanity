//
// Created by kingsli on 6/11/24.
//

#include "high_peer_server.h"

namespace vanity {

std::unordered_set<std::string> HighPeerServer::unknown_peers_in(const std::unordered_set<std::string>& peers) {
	std::unordered_set<std::string> unknown_peers;
	auto known_peers = peer_addresses();

	for (const auto& value : peers)
		if (not known_peers.contains(value))
			unknown_peers.insert(value);

	unknown_peers.erase(own_peer_addr());
	return unknown_peers;
}

void HighPeerServer::register_peer(Client &client, const std::string &addr, const std::string& peer_id) {
	PeerServer::register_peer(to_tcp(client), addr, peer_id);
}

void HighPeerServer::remove_peer(Client& client) {
	auto& tcp_client = to_tcp(client);
	forget_peer(tcp_client);
	tcp_client.close();
}

void HighPeerServer::peer_connect(const std::string &host, uint16_t port, const std::string &key, const std::string& peer_id, Client *client) {
	auto& peer = new_peer(host, port);
	auto id = post_plain(peer, peer_op_t::PEER_AUTH, key, own_peer_addr(), peer_id);
	add_auth_application(id, key, peer_id, client);
}

void HighPeerServer::peer_sync(Client &peer) {
	post(peer, peer_op_t::PEERS);
}

void HighPeerServer::request_peers(Client &client) {
	send(client, ok(peer_addresses()));
}

void HighPeerServer::post_request_peers(Context &ctx) {
	reply(ctx, peer_addresses());
}

void HighPeerServer::reply_request_peers(Context&, const std::unordered_set<std::string> &peers) {
	auto& opt_key = get_cluster_key();
	if (not opt_key)
		return;

	auto& opt_id = get_cluster_id();
	if (not opt_id)
		return;

	std::vector<std::pair<std::string, uint16_t>> unmade_peers;
	auto unknown_peers = unknown_peers_in(peers);
	unmade_peers.reserve(unknown_peers.size());

	for (const auto& value : unknown_peers)
		unmade_peers.emplace_back(unmake_address(value));

	auto& id = *opt_id;
	auto& key = *opt_key;
	for (const auto& [host, port] : unmade_peers)
		peer_connect(host, port, key, id);
}

void HighPeerServer::post_request_exit(Context &ctx) {
	client_close(ctx.client);
}

void HighPeerServer::reply_request_exit(Context &ctx) {
	throw std::runtime_error("received unexpected reply to exit request");
}

} // namespace vanity
