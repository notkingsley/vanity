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

	// the minimum length of a cluster id
	static constexpr auto M_MIN_CLUSTER_ID_LEN = 12;

	// the key of the cluster, if any
	std::optional<std::string> m_cluster_key;

	// the cluster id, if any
	std::optional<std::string> m_cluster_id;

	// the mutex for the cluster key
	std::mutex m_cluster_key_mutex;

protected:
	// make a random cluster id
	static std::string random_cluster_id();

	// check if a cluster key is long enough
	// returns true if the key is long enough, false otherwise
	static bool check_cluster_key_length(const std::string& key);

	// check if a cluster id is long enough
	// returns true if the id is long enough (or empty), false otherwise
	static bool check_cluster_id_length(const std::string& id);

	// validate a cluster key to authenticate a peer
	// returns true if the key is valid, false otherwise
	bool authenticate_cluster_key(const std::string& key);

	// sets the cluster key and id and returns the id
	// if the id is empty, a random id is chosen
	// this does not perform any checks
	const std::string& set_cluster_info(const std::string& key, const std::string& id);

	// checks if the server is in a cluster
	bool in_cluster();

	// leave the cluster, if in one
	void leave_cluster();

	// get the cluster key
	const std::optional<std::string>& get_cluster_key();

	// get the cluster id
	const std::optional<std::string>& get_cluster_id();
};

} // namespace vanity

#endif //VANITY_CLUSTER_AUTH_SERVER_H
