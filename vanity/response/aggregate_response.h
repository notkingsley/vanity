//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_AGGREGATE_RESPONSE_H
#define VANITY_AGGREGATE_RESPONSE_H

#include "response.h"

namespace vanity {

/*
 * An AggregateResponse collects several responses into one
 */
class AggregateResponse : public Response
{
public:
	// construct an AggregateResponse with an expected size and an aggregate type
	explicit AggregateResponse(size_t size) {
		*this << ok << ":AGG";
		serialize_length(size);
	};

	// append a response to the PipedResponse
	void append(const Response& response) {
		auto body = response.body();
		reserve(body.size());
		*this << body.data();
	};
};

} // namespace vanity

#endif //VANITY_AGGREGATE_RESPONSE_H
