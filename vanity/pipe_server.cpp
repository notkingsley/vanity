//
// Created by kingsli on 11/15/23.
//

#include "client/aggregating_client.h"
#include "client/tcp_client.h"
#include "pipe_server.h"

namespace vanity {

/*
 * A PipedClient is an AggregatingClient that writes to a pipe
 * This exists mainly to distinguish between a client that is
 * responding to a pipe and other AggregatingClients
 */
class PipedClient : public AggregatingClient
{
public:
	// create a PipedClient with an underlying client and a pipe size
	PipedClient(Client& client, size_t pipe_size)
		: AggregatingClient(client, pipe_size) {};
};

void PipeServer::request_pipe(Client &client, Request& request) {
	size_t len = request.get_len();
	PipedClient piped_client{client, len};
	do_handle(piped_client, request, len);
	piped_client.perform_write(*this);
}

void PipeServer::request_exit(Client &client) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("EXIT command not allowed in PIPE request"));

	if (auto tcp_client = dynamic_cast<TcpClient*>(&client))
		tcp_client->close();
	else
		throw std::runtime_error("request_exit: client is not a TcpClient");
}

void PipeServer::request_terminate(Client &client) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("TERMINATE command not allowed in PIPE request"));

	terminate();
}

} // namespace vanity