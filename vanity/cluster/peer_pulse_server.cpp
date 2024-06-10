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
	auto now = std::chrono::steady_clock::now();
	for (auto& client : get_peers()) {
		auto& peer_data = session_peer_data(*client);

		if (now - peer_data.last_pulse > M_MAX_PULSE_DELAY)
			kick_peer(*client);
	}
}

void PeerPulseServer::kick_peer(Client& peer) {
	// TODO: implement
}

PeerPulseServer::PeerPulseServer() {
	repeat(server_event::pulse, duration_cast<std::chrono::microseconds>(M_PULSE_INTERVAL));
}

void PeerPulseServer::async_request_pulse(Client& client) {
	session_peer_data(client).last_pulse = std::chrono::steady_clock::now();
}

void PeerPulseServer::event_pulse() {
	if (not in_cluster())
		return;

	send_pulses();
	check_pulses();
}

} // namespace vanity
