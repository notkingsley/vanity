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
	clear_auth_applications(error("already in a cluster"));
	m_cluster_key = key;
}

void ClusterServer::request_cluster_join(Client &client, const std::string& key, const std::string &host, uint16_t port) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (m_cluster_key)
		return send(client, error("already in a cluster"));

	auto& peer = peer_connect(host, port);
	auto id = post_plain(peer, peer_op_t::PEER_AUTH, key, own_peer_addr());
	add_auth_application(id, key, &client);
}

void ClusterServer::request_cluster_leave(Client &client) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (not m_cluster_key)
		return send(client, error("not in a cluster"));

	clear_peers();
	clear_auth_applications(error("cluster left"));
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

	clear_auth_applications(error("already in a cluster"));
	m_cluster_key = key;
	send(client, ok(*m_cluster_key));
}

void ClusterServer::request_peer_auth(Client &client, int64_t id, const std::string &key, const std::string& addr) {
	Context ctx {id, client};

	if (validate_cluster_key(key)) {
		register_peer(client, addr);
		reply(ctx, "OK");
	}
	else {
		client_close(client);
		reply(ctx, "DENIED");
	}
}

void ClusterServer::post_request_peer_auth(Context&, const std::string&, const std::string&) {
	throw std::runtime_error("a peer_auth request was received from a peer");
}

void ClusterServer::reply_request_peer_auth(Context& ctx, const std::string &data) {
	auto pending = pop_auth_application(ctx.msg_id);
	if (not pending)
		return remove_peer(ctx.client);

	if (data == "OK") {
		set_cluster_key(pending->key);
		if (pending->client)
			send(*pending->client, ok());

		post(ctx.client, peer_op_t::PING);
	}
	else if (data == "DENIED") {
		if (pending->client)
			send(*pending->client, error("peer denied connection"));

		remove_peer(ctx.client);
	}
	else {
		// TODO: report peer
	}
}

} // namespace vanity
