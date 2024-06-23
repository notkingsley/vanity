//
// Created by kingsli on 5/5/24.
//

#ifndef VANITY_PEER_AUTH_APPLICATION_SERVER_H
#define VANITY_PEER_AUTH_APPLICATION_SERVER_H

#include <optional>
#include <unordered_map>

#include "abstract_server.h"
#include "peer_operations.h"


namespace vanity {

/*
 * A PeerAuthApplicationServer tracks requests sent as
 * applications for peer authentication
 */
class PeerAuthApplicationServer : public virtual AbstractServer
{
protected:
	// a pending application we've sent as a peer_auth request
	struct PendingPeerAuth {
		// the key of the application
		std::string key;

		// the id we'll have in the cluster
		std::string own_id;

		// the client that sent the request to join the cluster, if any
		Client* client;
	};

	// all pending peer_auth requests
	std::unordered_map<msg_id_t, PendingPeerAuth> m_pending_peer_auths;

	// the mutex for the pending peer_auth requests
	std::mutex m_pending_peer_auths_mutex;

	// add a peer_auth request to the pending requests
	void add_auth_application(msg_id_t id, const std::string& key, const std::string &own_id, Client* client);

	// pop a pending peer_auth request
	std::optional<PendingPeerAuth> pop_auth_application(msg_id_t id);

	// clear all pending peer_auth requests
	// sending a copy of Sendable to each client, if any
	void clear_auth_applications(const Sendable& sendable);
};

} // namespace vanity

#endif //VANITY_PEER_AUTH_APPLICATION_SERVER_H
