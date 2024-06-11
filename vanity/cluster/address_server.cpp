//
// Created by kingsli on 6/11/24.
//

#include "address_server.h"

namespace vanity {

std::string AddressServer::make_address(const std::string &host, uint16_t port) {
	return host + ":" + std::to_string(port);
}

std::pair<std::string, uint16_t> AddressServer::unmake_address(const std::string &addr) {
	auto pos = addr.rfind(':');
	if (pos == std::string::npos)
		throw std::runtime_error("invalid address");

	auto host = addr.substr(0, pos);
	auto port = std::stoi(addr.substr(pos + 1));
	if (not validate_port(port))
		throw std::runtime_error("invalid port");

	return {host, static_cast<uint16_t>(port)};
}

std::optional<std::pair<std::string, uint16_t>> AddressServer::try_unmake_address(const std::string &addr) {
	try {
		return unmake_address(addr);
	} catch (const std::exception&) {
		return std::nullopt;
	}
}

std::string AddressServer::own_peer_addr() const {
	auto [host, port] = cluster_addr();
	return make_address(host, port);
}

} // namespace vanity
