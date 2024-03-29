//
// Created by kingsli on 12/21/23.
//

#include "proxy_client.h"

namespace vanity {

ProxyClient::ProxyClient(Client &client)  : m_client(client) {}

void ProxyClient::ready(SocketServer &server) {
	return m_client.ready(server);
}

bool ProxyClient::has_perm(operation_t op) const {
	return m_client.has_perm(op);
}

struct session_info &ProxyClient::session_info() {
	return m_client.session_info();
}

void ProxyClient::close() {
	m_client.close();
}

size_t ProxyClient::read(char *buffer, size_t buffer_size) const {
	return m_client.read(buffer, buffer_size);
}

void ProxyClient::write(SocketServer &server, Response &&response) {
	m_client.write(server, std::move(response));
}

Client &ProxyClient::client() const {
	return m_client;
}

} // namespace vanity