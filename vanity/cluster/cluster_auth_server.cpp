//
// Created by kingsli on 5/6/24.
//

#include "cluster_auth_server.h"
#include "utils/random.h"


namespace vanity {

std::string ClusterAuthServer::random_cluster_id() {
	return random_string(15);
}

bool ClusterAuthServer::check_cluster_key_length(const std::string &key) {
	return key.size() >= M_MIN_CLUSTER_KEY_LEN;
}

bool ClusterAuthServer::check_cluster_id_length(const std::string &id) {
	return id.empty() or id.size() >= M_MIN_CLUSTER_ID_LEN;
}

bool ClusterAuthServer::authenticate_cluster_key(const std::string &key) {
	std::lock_guard lock{m_cluster_key_mutex};
	if (not m_cluster_key)
		return false;

	return key == *m_cluster_key;
}

const std::string &ClusterAuthServer::set_cluster_info(const std::string &key, const std::string &id) {
	std::lock_guard lock{m_cluster_key_mutex};
	clear_auth_applications(error("already in a cluster"));

	m_cluster_key = key;
	if (id.empty())
		m_cluster_id = random_cluster_id();
	else
		m_cluster_id = id;

	return *m_cluster_id;
}

bool ClusterAuthServer::in_cluster() {
	std::lock_guard lock{m_cluster_key_mutex};
	return m_cluster_key.has_value();
}

void ClusterAuthServer::leave_cluster() {
	std::lock_guard lock{m_cluster_key_mutex};
	clear_auth_applications(error("cluster left"));
	m_cluster_key.reset();
	m_cluster_id.reset();
}

const std::optional<std::string> &ClusterAuthServer::get_cluster_key() {
	std::lock_guard lock{m_cluster_key_mutex};
	return m_cluster_key;
}

const std::optional<std::string> &ClusterAuthServer::get_cluster_id() {
	std::lock_guard lock{m_cluster_key_mutex};
	return m_cluster_id;
}

} // namespace vanity