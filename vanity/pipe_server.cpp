//
// Created by kingsli on 11/15/23.
//

#include "client/aggregating_client.h"
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
		: AggregatingClient(client, pipe_size, "PIPE") {};
};


void PipeServer::request_pipe(Client &client, Request& request) {
	size_t len = request.get_len();
	PipedClient piped_client{client, len};

	for (size_t i = 0; i < len - 1; ++i)
		do_handle(piped_client, request, false, true);
	do_handle(piped_client, request, true, true);

	piped_client.perform_write(*this);
}

void PipeServer::request_add_user(Client &client, const std::string &username, const std::string &password) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("ADD_USER command not allowed in PIPE request"));
	SerialAuthServer::request_add_user(client, username, password);
}

void PipeServer::request_edit_user(Client &client, const std::string &username, client_auth auth_level) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("EDIT_USER command not allowed in PIPE request"));
	SerialAuthServer::request_edit_user(client, username, auth_level);
}

void PipeServer::request_del_user(Client &client, const std::string &username) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("DEL_USER command not allowed in PIPE request"));
	SerialAuthServer::request_del_user(client, username);
}

void PipeServer::request_auth(Client &client, const std::string &username, const std::string &password) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("AUTH command not allowed in PIPE request"));
	SerialAuthServer::request_auth(client, username, password);
}

void PipeServer::request_change_password(Client &client, const std::string &new_password) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("CHANGE_PASSWORD command not allowed in PIPE request"));
	SerialAuthServer::request_change_password(client, new_password);
}

void PipeServer::request_exit(Client &client) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("EXIT command not allowed in PIPE request"));
	RequestServer::request_exit(client);
}

void PipeServer::request_terminate(Client &client) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("TERMINATE command not allowed in PIPE request"));
	RequestServer::request_terminate(client);
}

} // namespace vanity