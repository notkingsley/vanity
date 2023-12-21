//
// Created by kingsli on 11/12/23.
//

#ifndef VANITY_PIPED_CLIENT_H
#define VANITY_PIPED_CLIENT_H

#include "fake_client.h"
#include "piped_response.h"

namespace vanity {

/*
 * A PipedClient is something like a fake client
 * It is used to hold responses to PIPE requests,
 * constructing the final response from a series of
 * responses to each request in the pipe, while
 * providing the same interface as a normal client
 */
class PipedClient : public FakeClient
{
private:
	// the response
	PipedResponse m_response;

public:
	// create a PipedClient with an underlying client and a pipe size
	explicit PipedClient(Client& client, size_t pipe_size);

	// write the response to the pipe instead of the client
	void write(SocketServer& server, Response&& response) override;

	// write the pipe response to the client
	void perform_write(SocketServer& server);
};

} // namespace vanity

#endif //VANITY_PIPED_CLIENT_H
