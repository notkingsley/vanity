//
// Created by kingsli on 5/18/24.
//

#include "peer_behaviour_server.h"

namespace vanity {

peer_data_t::peer_behaviour_score_t PeerBehaviourServer::report_score(report_t report) {
	switch (report) {
		case report_t::BAD_MESSAGE:
		case report_t::BAD_REQUEST:
		case report_t::BAD_REPLY:
			return 10;

		default:
			throw std::runtime_error("invalid report_t");
	}
}

void PeerBehaviourServer::report_peer(Client &peer, report_t report) {
	auto& score = session_peer_data(peer).behaviour_score;
	score += report_score(report);

	if (score >= M_MAX_SCORE)
		evict_misbehaving_peer(peer);
}

bool PeerBehaviourServer::evict_opinion_misbehaving(Client &peer) {
	return session_peer_data(peer).behaviour_score >= M_MAX_SCORE * 0.75;
}

} // namespace vanity
