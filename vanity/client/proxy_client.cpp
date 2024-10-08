//
// Created by kingsli on 12/21/23.
//

#include "proxy_client.h"

namespace vanity {

ProxyClient::ProxyClient(Client &client)  : m_client(client) {}

bool ProxyClient::has_perm(operation_t op) const {
	return m_client.has_perm(op);
}

struct session_info &ProxyClient::session_info() {
	return m_client.session_info();
}

void ProxyClient::write(WriteManager& manager, Sendable &&sendable) {
	m_client.write(manager, std::move(sendable));
}

Client &ProxyClient::client() const {
	return m_client;
}

} // namespace vanity