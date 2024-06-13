//
// Created by kingsli on 5/18/24.
//

#ifndef VANITY_PEER_BEHAVIOUR_SERVER_H
#define VANITY_PEER_BEHAVIOUR_SERVER_H

#include <unordered_map>

#include "session_server.h"

namespace vanity {

/*
 * A PeerBehaviourServer handles the behaviour of a peer
 *
 * This tracks the correct behaviour of all peers, to evict any misbehaving peers
 */
class PeerBehaviourServer: public virtual SessionServer
{
private:
	// the maximum score a peer can have before being evicted
	static constexpr auto M_MAX_SCORE = 100;

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
	static peer_data_t::peer_behaviour_score_t report_score(report_t report);

protected:
	// report a peer for misbehaviour
	void report_peer(Client& peer, report_t report);

	/*
	 * The virtuality of these functions are flipped
	 * because PeerBehaviourServer cannot inherit
	 * from PeerEvictionServer
	 */
	// evict a peer for misbehaving
	virtual void evict_misbehaving_peer(Client &peer) = 0;

	// check if we think a peer is misbehaving
	static bool evict_opinion_misbehaving(Client& peer);
};

} // namespace vanity

#endif //VANITY_PEER_BEHAVIOUR_SERVER_H
