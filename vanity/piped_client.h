//
// Created by kingsli on 11/12/23.
//

#ifndef VANITY_PIPED_CLIENT_H
#define VANITY_PIPED_CLIENT_H

#include "client.h"
#include "piped_response.h"

namespace vanity {

/*
 * A PipedClient is something like a fake client
 * It is used to hold responses to PIPE requests,
 * constructing the final response from a series of
 * responses to each request in the pipe, while
 * providing the same interface as a normal client
 */
class PipedClient : public Client
{
private:
	// the response
	PipedResponse m_response;

	// the actual client
	Client& m_client;

public:
	// create a PipedClient with an underlying client and a pipe size
	explicit PipedClient(Client& client, size_t pipe_size)
		: m_client(client), m_response(pipe_size) {};

	// the client has sent a message, and it is ready to be read
	void ready(SocketServer& server) override;

	// check if the client has permission to perform an op
	bool has_perm(operation_t op) const override;

	// set the client's active database index
	void db(int db) override;

	// get the client's active database index
	int db() const override;

	// update the client's auth level
	void set_auth(const client_auth& auth) override;

	// set the client's username
	void username(const std::string& username) override;

	// get the client's username
	const std::string& username() const override;

	// request to close the client, not effective immediately
	// but when the client makes a request
	void close() override;

	// read from the socket
	size_t read(char* buffer, size_t buffer_size) const override;

	// write the response to the pipe
	void write(SocketServer& server, Response&& response) override;

	// write the pipe response to the client
	void perform_write(SocketServer& server);
};

} // namespace vanity

#endif //VANITY_PIPED_CLIENT_H
