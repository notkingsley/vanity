//
// Created by kingsli on 4/16/24.
//

#include "cluster_server.h"

namespace vanity {

void ClusterServer::handle(const std::string &msg, Client &client) {
	if (session_is_peer(client))
		handle_peer(msg, client);
	else
		handle_user(msg, client);
}

void ClusterServer::handle_peer(const std::string &msg, Client &client) {
	throw std::runtime_error("not implemented");
}

TcpClient &ClusterServer::connect(const std::string &host, uint16_t port) {
	auto& client = add_client(TcpClient{socket::Socket::connect(host.c_str(), port)});
	m_peers[{host, port}] = &client;
	return client;
}

void ClusterServer::request_cluster_join(Client &client, const std::string &host, uint16_t port) {
	if (m_cluster_key)
		return send(client, error("already in a cluster"));

	throw std::runtime_error("not implemented");
	connect(host, port);
	send(client, ok());
}

void ClusterServer::request_cluster_leave(Client &client) {
	if (not m_cluster_key)
		return send(client, error("not in a cluster"));

	for (auto& [_, peer] : m_peers)
		remove_client(*peer);

	m_peers.clear();
	m_cluster_key.reset();
	send(client, ok());
}

void ClusterServer::request_cluster_key(Client &client) {
	if (m_cluster_key)
		send(client, ok(*m_cluster_key));
	else
		send(client, null());
}

void ClusterServer::request_cluster_new(Client &client, const std::string &key) {
	if (m_cluster_key)
		return send(client, error("already in a cluster"));

	if (key.size() < M_MIN_CLUSTER_KEY_LEN)
		return send(client, error("key is too short"));

	m_cluster_key = key;
	send(client, ok(*m_cluster_key));
}

void ClusterServer::request_peers(Client &client) {
	std::vector<std::string> peers;
	peers.reserve(m_peers.size());

	for (auto& [peer, _] : m_peers)
		peers.push_back(peer.first + ":" + std::to_string(peer.second));

	send(client, ok(peers));
}

} // namespace vanity
