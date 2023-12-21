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
	AggregateResponse(size_t size, std::string agg_type) {
		*this << ok << ':' << agg_type << '(' << std::to_string(size) << ')';
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
