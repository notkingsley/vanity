//
// Created by kingsli on 4/2/24.
//

#ifndef VANITY_CLIENT_MANAGER_H
#define VANITY_CLIENT_MANAGER_H

#include <functional>
#include <string>

#include "read_manager.h"


namespace vanity {

// forward declaration
class TcpClient;

/*
 * A ClientManager is an interface for managing clients
 */
class ClientManager : public virtual ReadManager
{
public:
	using handle_callback_t = std::function<void(const std::string&)>;

	// add a client
	// return a reference to the permanent client
	virtual TcpClient& add_client(TcpClient&& client) = 0;

	// remove a client
	virtual void remove_client(TcpClient& client) = 0;

	// get a callback for when a message is received
	virtual handle_callback_t handle_callback(TcpClient& client) = 0;
};

} // namespace vanity

#endif //VANITY_CLIENT_MANAGER_H
