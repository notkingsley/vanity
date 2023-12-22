//
// Created by kingsli on 12/22/23.
//

#ifndef VANITY_REQUEST_TRACKER_H
#define VANITY_REQUEST_TRACKER_H

#include "request.h"

namespace vanity {

/*
 * A RequestTracker allows a view into the exact command string
 * that was processed by the server
 */
class RequestTracker
{
private:
	// the request
	const Request& m_request;

	// start of the command
	size_t m_start;

public:
	explicit RequestTracker(const Request& request)
		: m_request(request), m_start(request.index()) {}

	// view the command string since the start of the command
	std::string_view view() const {
		return m_request.view(m_start, m_request.index());
	}
};

} // namespace vanity

#endif //VANITY_REQUEST_TRACKER_H
