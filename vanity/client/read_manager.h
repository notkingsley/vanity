//
// Created by kingsli on 4/4/24.
//

#ifndef VANITY_READ_MANAGER_H
#define VANITY_READ_MANAGER_H

namespace vanity {

namespace socket {

// forward declaration
class SocketReadHandler;

} // namespace socket

/*
 * A ReadManager is an interface for managing SocketReadHandlers
 */
class ReadManager
{
public:
	using SocketReadHandler = socket::SocketReadHandler;

	// virtual destructor
	virtual ~ReadManager() = default;

	// add a read_handler
	virtual void add_read_handler(SocketReadHandler& read_handler) = 0;

	// remove a read_handler
	virtual void remove_read_handler(SocketReadHandler& read_handler) = 0;
};

} // namespace vanity

#endif //VANITY_READ_MANAGER_H
