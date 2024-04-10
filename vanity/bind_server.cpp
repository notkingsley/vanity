//
// Created by kingsli on 4/11/24.
//

#include "bind_server.h"

namespace vanity {

BindServer::BindServer(std::vector<uint16_t> ports) : m_ports{std::move(ports)} { }

void BindServer::start() {
	m_listeners.reserve(m_ports.size());

	for (auto& port : m_ports) {
		m_listeners.emplace_back(port);
		epoll_add(m_listeners.back());
		logger().info("Listening on port " + std::to_string(port));
	}
}

void BindServer::stop() {
	m_listeners.clear();
}

} // namespace vanity
