//
// Created by kingsli on 5/6/24.
//

#ifndef VANITY_CLUSTER_AUTH_SERVER_H
#define VANITY_CLUSTER_AUTH_SERVER_H

#include "peer_auth_application_server.h"
#include "response/response_server.h"


namespace vanity {

/*
 * A ClusterAuthServer handles the cluster key for a server
 */
class ClusterAuthServer : public virtual PeerAuthApplicationServer, public virtual ResponseServer
{
private:
	// the minimum length of a cluster key
	static constexpr auto M_MIN_CLUSTER_KEY_LEN = 12;

	// the key of the cluster, if any
	std::optional<std::string> m_cluster_key;

	// the mutex for the cluster key
	std::mutex m_cluster_key_mutex;

protected:
	// check if a cluster key is long enough
	// returns true if the key is long enough, false otherwise
	static bool check_cluster_key_length(const std::string& key);

	// validate a cluster key to authenticate a peer
	// returns true if the key is valid, false otherwise
	bool authenticate_cluster_key(const std::string& key);

	// sets the cluster key and returns the key
	const std::string& set_cluster_key(const std::string& key);

	// checks if the server is in a cluster
	bool in_cluster();

	// leave the cluster, if in one
	void leave_cluster();

	// get the cluster key, throws if not in a cluster
	const std::string& get_cluster_key();

	// get the cluster key, return std::nullopt if not in a cluster
	const std::optional<std::string>& get_cluster_key_opt();
};

} // namespace vanity

#endif //VANITY_CLUSTER_AUTH_SERVER_H
