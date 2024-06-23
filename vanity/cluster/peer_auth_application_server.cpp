//
// Created by kingsli on 5/5/24.
//

#include "peer_auth_application_server.h"

namespace vanity {

void PeerAuthApplicationServer::add_auth_application(msg_id_t id, const std::string &key, const std::string &own_id, Client* client) {
	std::lock_guard lock{m_pending_peer_auths_mutex};
	m_pending_peer_auths.emplace(id, PendingPeerAuth{key, own_id, client});
}

auto PeerAuthApplicationServer::pop_auth_application(msg_id_t id) -> std::optional<PendingPeerAuth> {
	std::lock_guard lock{m_pending_peer_auths_mutex};
	auto it = m_pending_peer_auths.find(id);

	if (not m_pending_peer_auths.contains(id))
		return std::nullopt;

	auto pending = it->second;
	m_pending_peer_auths.erase(it);
	return pending;
}

void PeerAuthApplicationServer::clear_auth_applications(const Sendable& sendable) {
	std::lock_guard lock{m_pending_peer_auths_mutex};
	for (auto& [_, pending] : m_pending_peer_auths)
		if (pending.client)
			send(*pending.client, Sendable{sendable});

	m_pending_peer_auths.clear();
}

} // namespace vanity
