//
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_PIPED_RESPONSE_H
#define VANITY_PIPED_RESPONSE_H

#include "response.h"

namespace vanity {

/*
 * A PipedResponse is a response that is constructed
 * from a series of responses to PIPE requests
 */
class PipedResponse : public Response
{
public:
	// construct a PipedResponse with an expected size
	explicit PipedResponse(size_t size) {
		*this << "PIPE" << '(' << std::to_string(size) << ')';
	};

	// append a response to the PipedResponse
	void append(const Response& response) {
		auto body = response.body();
		reserve(body.size());
		*this << body.data();
	};
};

} // namespace vanity

#endif //VANITY_PIPED_RESPONSE_H
