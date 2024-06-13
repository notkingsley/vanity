//
// Created by kingsli on 6/10/24.
//

#include "peer_pulse_server.h"

namespace vanity {

void PeerPulseServer::send_pulses() {
	for (auto& client : get_peers())
		send_async(*client, async_op_t::PULSE);
}

void PeerPulseServer::check_pulses() {
	auto now = steady_clock::now();
	for (auto& client : get_peers())
		if (now - session_peer_data(*client).last_pulse > M_MAX_PULSE_DELAY)
			evict_dead_peer(*client);
}

bool PeerPulseServer::evict_opinion_dead(Client& peer) {
	return steady_clock::now() - session_peer_data(peer).last_pulse > M_MAX_PULSE_DELAY * 0.75;
}

std::chrono::seconds PeerPulseServer::forget_evict_issue_after() {
	return M_MAX_PULSE_DELAY * 2;
}

PeerPulseServer::PeerPulseServer() {
	repeat(server_event::pulse, duration_cast<std::chrono::microseconds>(M_PULSE_INTERVAL));
}

void PeerPulseServer::async_request_pulse(Client& client) {
	session_peer_data(client).last_pulse = steady_clock::now();
}

void PeerPulseServer::event_pulse() {
	if (not in_cluster())
		return;

	send_pulses();
	check_pulses();
}

} // namespace vanity
