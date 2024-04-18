//
// Created by kingsli on 4/16/24.
//

#include "cluster_server.h"
#include "global_log.h"


namespace vanity {

void ClusterServer::handle(const std::string &msg, Client &client) {
	if (session_is_peer(client))
		handle_peer(msg, client);
	else
		handle_user(msg, client);
}

void ClusterServer::handle_peer(const std::string &msg, Client &client) {
	if (msg == "OK") {
		return global_log("Connected to peer");
	}
	else if (msg == "DENIED") {
		m_cluster_key.reset();
		remove_peer(client);
		return global_log("Peer denied connection");
	}
	else {
		return global_log("Unknown message from peer: " + msg);
	}
}

bool ClusterServer::validate_cluster_key(const std::string &key) {
	if (not m_cluster_key)
		return false;

	return key == *m_cluster_key;
}

std::string ClusterServer::get_own_address() const {
	auto [host, port] = get_peer_host_and_port();
	return join_host_port(host, port);
}

TcpClient &ClusterServer::connect(const char* host, uint16_t port) {
	return add_client(TcpClient{socket::Socket::connect(host, port)});
}

TcpClient &ClusterServer::new_peer(const std::string &host, uint16_t port) {
	auto& peer = connect(host.c_str(), port);
	session_auth(peer) = client_auth::PEER;
	m_peers[&peer] = join_host_port(host, port);
	return peer;
}

void ClusterServer::remove_peer(Client& client) {
	auto& peer = to_tcp(client);
	m_peers.erase(&peer);
	remove_client(peer);
}

void ClusterServer::request_cluster_join(Client &client, const std::string& key, const std::string &host, uint16_t port) {
	if (m_cluster_key)
		return send(client, error("already in a cluster"));

	auto& peer = new_peer(host, port);
	send(peer, peer_auth(key, get_own_address()));

	m_cluster_key = key;
	send(client, ok());
}

void ClusterServer::request_cluster_leave(Client &client) {
	if (not m_cluster_key)
		return send(client, error("not in a cluster"));

	for (auto& [peer, _] : m_peers)
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

	for (auto& [_, peer_addr] : m_peers)
		peers.push_back(peer_addr);

	send(client, ok(peers));
}

void ClusterServer::request_peer_auth(Client &client, const std::string &key, const std::string& addr) {
	if (not validate_cluster_key(key))
		return send(client, denied());

	global_log("Peer authenticated: " + addr);
	session_auth(client) = client_auth::PEER;
	m_peers[&to_tcp(client)] = addr;
	send(client, ok());
}

Response ClusterServer::peer_auth(const std::string &key, const std::string& addr) {
	return (Response() << "PEER_AUTH").serialize_string_body(key).serialize_string_body(addr);
}

std::string ClusterServer::join_host_port(const std::string &host, uint16_t port) {
	return host + ":" + std::to_string(port);
}

TcpClient& ClusterServer::to_tcp(Client &client) {
	if (auto tcp = dynamic_cast<TcpClient*>(&client))
		return *tcp;
	else
		throw std::runtime_error("expected a TcpClient");
}

} // namespace vanity
