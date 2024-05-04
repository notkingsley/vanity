//
// Created by kingsli on 4/16/24.
//

#include "cluster_server.h"
#include "utils/global_log.h"


namespace vanity {

void ClusterServer::handle(const std::string &msg, Client &client) {
	if (session_is_peer(client))
		handle_peer(msg, client);
	else
		handle_user(msg, client);
}

bool ClusterServer::validate_cluster_key(const std::string &key) {
	if (not m_cluster_key)
		return false;

	return key == *m_cluster_key;
}

std::string ClusterServer::get_own_address() const {
	auto [host, port] = cluster_addr();
	return make_address(host, port);
}

TcpClient &ClusterServer::connect(const std::string &host, uint16_t port) {
	return add_client(TcpClient{socket::Socket::connect(host.c_str(), port)});
}

void ClusterServer::request_cluster_join(Client &client, const std::string& key, const std::string &host, uint16_t port) {
	if (m_cluster_key)
		return send(client, error("already in a cluster"));

	auto& peer = connect(host, port);
	post_plain(peer, peer_op_t::PEER_AUTH, key, get_own_address());
	register_peer(peer, make_address(host, port));

	m_cluster_key = key;
	send(client, ok());
}

void ClusterServer::request_cluster_leave(Client &client) {
	if (not m_cluster_key)
		return send(client, error("not in a cluster"));

	clear_peers();
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

void ClusterServer::request_peer_auth(Client &client, int64_t id, const std::string &key, const std::string& addr) {
	Context ctx {id, client};

	if (not validate_cluster_key(key)) {
		client_close(client);
		return reply(ctx, "DENIED");
	}

	global_log("Peer authenticated: " + addr);
	register_peer(client, addr);
	reply(ctx, "OK");
}

void ClusterServer::post_request_peer_auth(Context&, const std::string&, const std::string&) {
	throw std::runtime_error("a peer_auth request was received from a peer");
}

void ClusterServer::reply_request_peer_auth(Client &client, const std::string &data) {
	if (data == "OK") {
		global_log("Connected to peer");
		post(client, peer_op_t::PING);
	}
	else if (data == "DENIED") {
		global_log("Peer denied connection");
		m_cluster_key.reset();
		remove_peer(client);
	}
	else {
		// TODO: report peer
	}
}

std::string ClusterServer::make_address(const std::string &host, uint16_t port) {
	return host + ":" + std::to_string(port);
}

} // namespace vanity
