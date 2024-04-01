//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_AGGREGATING_CLIENT_H
#define VANITY_AGGREGATING_CLIENT_H

#include "proxy_client.h"
#include "response/aggregate_response.h"

namespace vanity {

/*
 * An AggregatingClient is a ProxyClient that aggregates responses
 * It holds responses to a given type of request,
 * constructing the final response from a series of
 * responses to each request in the series, while
 * providing the same interface as a normal client
 */
class AggregatingClient : public ProxyClient
{
private:
	// the response
	AggregateResponse m_response;

public:
	// create a AggregatingClient with an underlying client, a response size and an aggregate type
	AggregatingClient(Client& client, size_t resp_size)
		: ProxyClient(client), m_response(resp_size) {}

	// write the response to the aggregate instead of the client
	void write(WriteManager& manager, Response&& response) override {
		m_response.append(response);
	}

	// write the aggregate response to the client
	void perform_write(WriteManager& manager) {
		client().write(manager, m_response.move());
	}
};

} // namespace vanity

#endif //VANITY_AGGREGATING_CLIENT_H
