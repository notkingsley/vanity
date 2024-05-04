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
	std::lock_guard lock{m_cluster_key_mutex};
	if (not m_cluster_key)
		return false;

	return key == *m_cluster_key;
}

void ClusterServer::set_cluster_key(const std::string &key) {
	std::lock_guard lock{m_cluster_key_mutex};
	clear_auth_applications();
	m_cluster_key = key;
}

void ClusterServer::cluster_join(Client& client, const std::string &key, const std::string &host, uint16_t port) {
	auto& peer = connect(host, port);
	auto id = post_plain(peer, peer_op_t::PEER_AUTH, key, get_own_address());
	register_peer(peer, make_address(host, port));
	add_auth_application(id, key, client);
}

std::string ClusterServer::get_own_address() const {
	auto [host, port] = cluster_addr();
	return make_address(host, port);
}

TcpClient &ClusterServer::connect(const std::string &host, uint16_t port) {
	return add_client(TcpClient{socket::Socket::connect(host.c_str(), port)});
}

void ClusterServer::request_cluster_join(Client &client, const std::string& key, const std::string &host, uint16_t port) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (m_cluster_key)
		return send(client, error("already in a cluster"));

	cluster_join(client, key, host, port);
}

void ClusterServer::request_cluster_leave(Client &client) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (not m_cluster_key)
		return send(client, error("not in a cluster"));

	clear_peers();
	m_cluster_key.reset();
	send(client, ok());
}

void ClusterServer::request_cluster_key(Client &client) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (m_cluster_key)
		send(client, ok(*m_cluster_key));
	else
		send(client, null());
}

void ClusterServer::request_cluster_new(Client &client, const std::string &key) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (m_cluster_key)
		return send(client, error("already in a cluster"));

	if (key.size() < M_MIN_CLUSTER_KEY_LEN)
		return send(client, error("key is too short"));

	set_cluster_key(key);
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

void ClusterServer::reply_request_peer_auth(Context& ctx, const std::string &data) {
	auto pending = pop_auth_application(ctx.msg_id);
	if (not pending)
		return remove_peer(ctx.client);

	if (data == "OK") {
		global_log("Connected to peer");
		set_cluster_key(pending->key);
		send(pending->client, ok());
		post(ctx.client, peer_op_t::PING);
	}
	else if (data == "DENIED") {
		global_log("Peer denied connection");
		send(pending->client, error("peer denied connection"));
		remove_peer(ctx.client);
	}
	else {
		// TODO: report peer
	}
}

std::string ClusterServer::make_address(const std::string &host, uint16_t port) {
	return host + ":" + std::to_string(port);
}

void ClusterServer::add_auth_application(msg_id_t id, const std::string &key, Client &client) {
	std::lock_guard lock{m_pending_peer_auths_mutex};
	m_pending_peer_auths.emplace(id, PendingPeerAuth{key, client});
}

auto ClusterServer::pop_auth_application(msg_id_t id) -> std::optional<PendingPeerAuth> {
	std::lock_guard lock{m_pending_peer_auths_mutex};
	auto it = m_pending_peer_auths.find(id);

	if (not m_pending_peer_auths.contains(id))
		return std::nullopt;

	auto pending = it->second;
	m_pending_peer_auths.erase(it);
	return pending;
}

void ClusterServer::clear_auth_applications() {
	std::lock_guard lock{m_pending_peer_auths_mutex};
	for (auto& [_, pending] : m_pending_peer_auths)
		send(pending.client, error("already in a cluster"));

	m_pending_peer_auths.clear();
}

} // namespace vanity
