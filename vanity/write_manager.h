//
// Created by kingsli on 4/1/24.
//

#ifndef VANITY_WRITE_MANAGER_H
#define VANITY_WRITE_MANAGER_H

namespace vanity {

namespace socket {

// forward declaration
class SocketWriter;

} // namespace socket

/*
 * A WriteManager is an interface for managing writers
 */
class WriteManager
{
public:
	using SocketWriter = socket::SocketWriter;

	// virtual destructor
	virtual ~WriteManager() = default;

	// add a writer
	virtual void add_writer(SocketWriter& writer) = 0;

	// remove a writer
	virtual void remove_writer(SocketWriter& writer) = 0;
};

} // namespace vanity

#endif //VANITY_WRITE_MANAGER_H
