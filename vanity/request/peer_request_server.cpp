//
// Created by kingsli on 4/29/24.
//

#include "peer_request_server.h"

namespace vanity {

void PeerRequestServer::handle_peer_request(PeerRequest &request, Client &client) {
	auto request_t = request.get_request_t();

	switch (request_t) {
		case peer_request_t::POST: {
			PostRequest post_request{request};
			handle_post_request(post_request, client);
			break;
		}
		case peer_request_t::REPLY: {
			throw std::runtime_error("REPLY not implemented");
		}
	}
}

void PeerRequestServer::handle_peer(const std::string &msg, Client &client) {
	PeerRequest request{msg};
	handle_peer_request(request, client);
}

} // namespace vanity
