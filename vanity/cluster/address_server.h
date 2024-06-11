//
// Created by kingsli on 6/11/24.
//

#ifndef VANITY_ADDRESS_SERVER_H
#define VANITY_ADDRESS_SERVER_H

#include "bind_server.h"

namespace vanity {

/*
 * An AddressServer manages trivial logic related to addresses
 */
class AddressServer : public virtual BindServer
{
protected:
	// make an address by joining a host and a port into a single string
	static std::string make_address(const std::string& host, uint16_t port);

	// unmake an address by splitting a host and a port from a single string
	static std::pair<std::string, uint16_t> unmake_address(const std::string& addr);

	// try to unmake an address from a string, returning nullopt if the address is invalid
	static std::optional<std::pair<std::string, uint16_t>> try_unmake_address(const std::string& addr);

	// get this server's peer address as a single string
	std::string own_peer_addr() const;
};

} // namespace vanity

#endif //VANITY_ADDRESS_SERVER_H
