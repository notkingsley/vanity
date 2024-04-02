//
// Created by kingsli on 4/2/24.
//

#ifndef VANITY_CLIENT_MANAGER_H
#define VANITY_CLIENT_MANAGER_H

#include <functional>
#include <string>

namespace vanity {

// forward declaration
class ConcreteClient;

/*
 * A ClientManager is an interface for managing clients
 */
class ClientManager
{
public:
	using handle_callback_t = std::function<void(const std::string&)>;

	// virtual destructor
	virtual ~ClientManager() = default;

	// add a client
	virtual void add_client(ConcreteClient&& client) = 0;

	// remove a client
	virtual void remove_client(ConcreteClient& client) = 0;

	// get a callback for when a message is received
	virtual handle_callback_t handle_callback(ConcreteClient& client) = 0;
};

} // namespace vanity

#endif //VANITY_CLIENT_MANAGER_H
