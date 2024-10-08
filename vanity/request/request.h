//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_REQUEST_H
#define VANITY_REQUEST_H

#include "extractable.h"
#include "client/operations.h"

namespace vanity {

/*
 * A Request is a class that holds information
 * about a request string currently being processed
 */
class Request : public Extractable
{
public:
	// create a Request with a request string
	explicit Request(const std::string& msg);

	// formats the request string and position for logging
	std::string format() const;

	// get the index of the current position in the request string
	size_t index() const;

	// get a view of the request string between two indices
	std::string_view view(size_t start, size_t end) const;

	// extract an object type from the extractable
	// currently unused
	object_t get_object_t();

	// extract an operation type from the extractable
	operation_t get_operation();

	// like get_operation, but doesn't advance the position
	operation_t peek_operation();
};

} // namespace vanity

#endif //VANITY_REQUEST_H
