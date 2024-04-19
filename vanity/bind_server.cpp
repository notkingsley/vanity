//
// Created by kingsli on 4/11/24.
//

#include "bind_server.h"

#include <iostream>


namespace vanity {

BindServer::BindServer(std::string host, std::vector<uint16_t> ports, uint16_t cluster_port):
	m_ports{std::move(ports)}, m_cluster_port{cluster_port}, m_host{std::move(host)} {}

void BindServer::start() {
	auto ports = m_ports;
	ports.push_back(m_cluster_port);
	m_listeners.reserve(ports.size());

	for (auto& port : ports) {
		m_listeners.emplace_back(m_host.c_str(), port);
		epoll_add(m_listeners.back());
		logger().info("Listening on port " + std::to_string(port));
	}
}

void BindServer::stop() {
	m_listeners.clear();
}

std::pair<std::string, uint16_t> BindServer::get_peer_host_and_port() const {
	return m_listeners.back().get_host_and_port();
}

} // namespace vanity
