//
// Created by kingsli on 5/6/24.
//

#include "cluster_auth_server.h"

namespace vanity {

bool ClusterAuthServer::authenticate_cluster_key(const std::string &key) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (not m_cluster_key)
		return false;

	return key == *m_cluster_key;
}

bool ClusterAuthServer::check_cluster_key_length(const std::string &key) {
	return key.size() >= M_MIN_CLUSTER_KEY_LEN;
}

const std::string& ClusterAuthServer::set_cluster_key(const std::string &key) {
	std::lock_guard lock{m_cluster_key_mutex};
	clear_auth_applications(error("already in a cluster"));
	m_cluster_key = key;
	return *m_cluster_key;
}

bool ClusterAuthServer::in_cluster() {
	std::lock_guard lock{m_cluster_key_mutex};
	return m_cluster_key.has_value();
}

void ClusterAuthServer::leave_cluster() {
	std::lock_guard lock{m_cluster_key_mutex};
	clear_auth_applications(error("cluster left"));
	m_cluster_key.reset();
}

const std::string &ClusterAuthServer::get_cluster_key() {
	std::lock_guard lock{m_cluster_key_mutex};
	if (not m_cluster_key)
		throw std::runtime_error("not in a cluster");

	return *m_cluster_key;
}

const std::optional<std::string> &ClusterAuthServer::get_cluster_key_opt() {
	std::lock_guard lock{m_cluster_key_mutex};
	return m_cluster_key;
}

} // namespace vanity