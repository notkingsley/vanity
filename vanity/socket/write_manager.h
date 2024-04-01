//
// Created by kingsli on 4/1/24.
//

#ifndef VANITY_WRITE_MANAGER_H
#define VANITY_WRITE_MANAGER_H

#include <stdexcept> // TODO: remove this


namespace vanity::socket {

// forward declaration
class SocketWriter;

/*
 * A WriteManager is an interface for managing writers
 */
class WriteManager
{
public:
	// add a writer
	virtual void add_writer(SocketWriter& writer) = 0;

	// remove a writer
	virtual void remove_writer(SocketWriter& writer) {
		// TODO: remove this
		throw std::runtime_error("remove_writer not implemented");
	};
};

} // namespace vanity::socket

#endif //VANITY_WRITE_MANAGER_H
