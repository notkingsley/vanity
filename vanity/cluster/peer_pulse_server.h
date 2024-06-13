//
// Created by kingsli on 6/10/24.
//

#ifndef VANITY_PEER_PULSE_SERVER_H
#define VANITY_PEER_PULSE_SERVER_H

#include "peer_eviction_server.h"
#include "repeat_event_server.h"


namespace vanity {

/*
 * A PeerPulseServer sends periodic pulses to peers
 * and tracks the pulses of peers
 */
class PeerPulseServer : public virtual PeerEvictionServer, public virtual RepeatEventServer
{
private:
	using steady_clock = std::chrono::steady_clock;

	// the time between pulses
	static constexpr auto M_PULSE_INTERVAL = std::chrono::seconds(10);

	// the time before a peer is considered dead
	static constexpr auto M_MAX_PULSE_DELAY = std::chrono::seconds(60);

	// send a pulse to all peers
	void send_pulses();

	// check if any peers have not pulsed in a while
	void check_pulses();

protected:
	// check if we think a peer is dead
	bool evict_opinion_dead(Client& peer) override;

	// get how long to wait before forgetting an issue
	std::chrono::seconds forget_evict_issue_after() override;

public:
	// create a new PeerPulseServer and start the pulse event
	PeerPulseServer();

	// a pulse async request was received from a peer
	void async_request_pulse(Client& client) override;

	// a pulse event was received
	void event_pulse() override;
};

}

#endif //VANITY_PEER_PULSE_SERVER_H
