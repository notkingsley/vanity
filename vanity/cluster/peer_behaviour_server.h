//
// Created by kingsli on 5/18/24.
//

#ifndef VANITY_PEER_BEHAVIOUR_SERVER_H
#define VANITY_PEER_BEHAVIOUR_SERVER_H

#include <unordered_map>

#include "client/client.h"

namespace vanity {

/*
 * A PeerBehaviourServer handles the behaviour of a peer
 *
 * This tracks the correct behaviour of all peers, to evict any misbehaving peers
 */
class PeerBehaviourServer
{
private:
	using behaviour_score_t = uint;

	// the maximum score a peer can have before being evicted
	static constexpr auto M_MAX_SCORE = 100;

	// all misbehaving peers
	std::unordered_map<Client*, behaviour_score_t> m_reported_peers;

	// the mutex for the reported peers
	std::mutex m_reported_peers_mutex;

	// check if a peer should be evicted
	// assumes the reported peers mutex is locked
	void check_peer_eviction(Client& peer);

	// evict a peer
	void evict_peer(Client& peer);

protected:
	// all the bad things a peer can do
	enum class report_t
	{
		// the peer sent a malformed message
		BAD_MESSAGE,
		// the peer sent an invalid request
		BAD_REQUEST,
		// the peer sent an invalid reply
		BAD_REPLY,
	};

private:
	// get the score for a report
	static behaviour_score_t report_score(report_t report);

protected:
	// report a peer for misbehaviour
	void report_peer(Client& peer, report_t report);
};

} // namespace vanity

#endif //VANITY_PEER_BEHAVIOUR_SERVER_H
