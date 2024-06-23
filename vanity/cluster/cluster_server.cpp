//
// Created by kingsli on 4/16/24.
//

#include "cluster_server.h"

namespace vanity {

bool ClusterServer::is_on_cluster_port(Client &client) const {
	return cluster_addr().second == to_tcp(client).local_addr().second;
}

void ClusterServer::handle(const std::string &msg, Client &client) {
	if (session_is_peer(client))
		handle_peer(msg, client);
	else
		handle_user(msg, client);
}

void ClusterServer::request_cluster_join(Client &client, const std::string& key, const std::string& id, const std::string &host, uint16_t port) {
	if (in_cluster())
		return send(client, error("already in a cluster"));

	auto own_id = id;
	if (own_id.empty())
		own_id = random_cluster_id();
	else if (not check_cluster_id_length(own_id))
		return send(client, error("id is too short"));

	peer_connect(host, port, key, own_id, &client);
}

void ClusterServer::request_cluster_leave(Client &client) {
	if (not in_cluster())
		return send(client, error("not in a cluster"));

	leave_cluster();
	clear_peers();
	send(client, ok());
}

void ClusterServer::request_cluster_key(Client &client) {
	if (auto& key = get_cluster_key())
		send(client, ok(*key));
	else
		send(client, null());
}

void ClusterServer::request_cluster_new(Client &client, const std::string &key, const std::string& id) {
	if (in_cluster())
		return send(client, error("already in a cluster"));

	if (not check_cluster_key_length(key))
		return send(client, error("key is too short"));

	if (not check_cluster_id_length(id))
		return send(client, error("id is too short"));

	send(client, ok(set_cluster_info(key, id)));
}

void ClusterServer::request_peer_auth(Client &client, int64_t id, const std::string &key, const std::string& addr, const std::string& peer_id) {
	Context ctx {id, client};

	if (not is_on_cluster_port(client)) {
		client_close(client);
		return reply(ctx, own_peer_addr());
	}

	if (not authenticate_cluster_key(key)) {
		client_close(client);
		return reply(ctx, "DENIED");
	}

	auto& own_id = get_cluster_id();
	if (not own_id) {
		client_close(client);
		return reply(ctx, "DENIED");
	}

	register_peer(client, addr, peer_id);
	reply(ctx, "OK" + *own_id);
}

void ClusterServer::post_request_peer_auth(Context&, const std::string&, const std::string&) {
	throw std::runtime_error("a peer_auth request was received from a peer");
}

void ClusterServer::reply_request_peer_auth(Context& ctx, const std::string &data) {
	auto pending = pop_auth_application(ctx.msg_id);
	if (not pending)
		return remove_peer(ctx.client);

	if (data.starts_with("OK")) {
		session_id(ctx.client) = data.substr(2);
		set_cluster_info(pending->key, pending->own_id);
		if (pending->client)
			send(*pending->client, ok(pending->own_id));

		peer_sync(ctx.client);
	}
	else if (data == "DENIED") {
		if (pending->client)
			send(*pending->client, error("peer denied connection"));

		remove_peer(ctx.client);
	}
	else if (auto addr = try_unmake_address(data)) {
		remove_peer(ctx.client);
		peer_connect(addr->first, addr->second, pending->key, pending->own_id, pending->client);
	}
	else {
		report_peer(ctx.client, report_t::BAD_REPLY);
	}
}

} // namespace vanity
