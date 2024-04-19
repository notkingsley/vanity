//
// Created by kingsli on 4/11/24.
//

#include "bind_server.h"

#include <iostream>


namespace vanity {

BindServer::BindServer(std::string host, const std::vector<uint16_t>& ports, uint16_t cluster_port):
	m_ports{ports.begin(), ports.end()}, m_cluster_port{cluster_port}, m_host{std::move(host)} {}

void BindServer::start() {
	std::vector<uint16_t> ports {m_ports.begin(), m_ports.end()};
	ports.push_back(m_cluster_port);
	m_listeners.reserve(ports.size());

	for (auto& port : ports)
		bind(m_host, port);
}

void BindServer::stop() {
	m_listeners.clear();
}

std::pair<std::string, uint16_t> BindServer::get_peer_host_and_port() const {
	return m_listeners.find({m_host, m_cluster_port})->second->get_host_and_port();
}

void BindServer::request_bind(Client &client, const std::string &host, int64_t port) {
	if (not validate_port(port))
		return send(client, error("invalid port"));

	if (m_listeners.contains({host, port}))
		return send(client, error("already bound to port"));

	bind(host, port);
	send(client, ok());
}

void BindServer::request_unbind(Client &client, const std::string &host, int64_t port) {
	if (not validate_port(port))
		return send(client, error("invalid port"));

	if (m_listeners.size() == 1)
		return send(client, error("cannot unbind last port"));

	if (port == m_cluster_port and host == m_host)
		return send(client, error("cannot unbind cluster port"));

	if (m_listeners.erase({host, port}) == 0)
		return send(client, error("was not bound to port"));

	send(client, ok());
}

bool BindServer::validate_port(int64_t port) {
	return port > 0 and port <= 65535;
}

void BindServer::bind(const std::string &host, uint16_t port) {
	auto it = m_listeners.insert({{host, port}, std::make_unique<SocketListener>(host.c_str(), port)}).first;
	epoll_add(*it->second);

	logger().info("Listening on " + host + ":" + std::to_string(port));
}

} // namespace vanity
