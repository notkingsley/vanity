//
// Created by kingsli on 5/18/24.
//

#include "peer_behaviour_server.h"

namespace vanity {

void PeerBehaviourServer::check_peer_eviction(Client &peer) {
	if (m_reported_peers[&peer] >= M_MAX_SCORE)
		evict_peer(peer);
}

void PeerBehaviourServer::evict_peer(Client &peer) { }

auto PeerBehaviourServer::report_score(report_t report) -> behaviour_score_t {
	switch (report) {
		case report_t::BAD_MESSAGE:
		case report_t::BAD_REQUEST:
		case report_t::BAD_REPLY:
			return 10;
	}
}

void PeerBehaviourServer::report_peer(Client &peer, report_t report) {
	std::lock_guard lock{m_reported_peers_mutex};
	m_reported_peers[&peer] += report_score(report);
	check_peer_eviction(peer);
}

} // namespace vanity
