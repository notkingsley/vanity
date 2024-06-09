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

void ClusterServer::request_cluster_join(Client &client, const std::string& key, const std::string &host, uint16_t port) {
	if (in_cluster())
		return send(client, error("already in a cluster"));

	peer_connect(host, port, key, &client);
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

void ClusterServer::request_cluster_new(Client &client, const std::string &key) {
	if (in_cluster())
		return send(client, error("already in a cluster"));

	if (not check_cluster_key_length(key))
		return send(client, error("key is too short"));

	send(client, ok(set_cluster_key(key)));
}

void ClusterServer::request_peer_auth(Client &client, int64_t id, const std::string &key, const std::string& addr) {
	Context ctx {id, client};

	if (authenticate_cluster_key(key)) {
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

		peer_sync(ctx.client);
	}
	else if (data == "DENIED") {
		if (pending->client)
			send(*pending->client, error("peer denied connection"));

		remove_peer(ctx.client);
	}
	else {
		report_peer(ctx.client, report_t::BAD_REPLY);
	}
}

} // namespace vanity
