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
	connect(host, port);
}

} // namespace vanity
