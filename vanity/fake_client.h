//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_FAKE_CLIENT_H
#define VANITY_FAKE_CLIENT_H

#include "client.h"

namespace vanity {

/*
 * A FakeClient implements the Client interface, while keeping
 * a reference to another Client&, forwarding all calls
 * as a default implementation
 */
class FakeClient : public Client
{
private:
	// the actual client
	Client& m_client;

public:
	// create a FakeClient with an underlying client
	explicit FakeClient(Client& client);

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

	// get the client's current connection state
	conn_state state() const override;

	// set the client's current connection state
	void state(conn_state state) override;

	// get a reference to the client's current transaction data
	// if the client is not in a transaction, this will throw
	transaction_data& get_transaction_data() override;

	// get the client's username
	const std::string& username() const override;

	// request to close the client, not effective immediately
	// but when the client makes a request
	void close() override;

	// read from the socket
	size_t read(char* buffer, size_t buffer_size) const override;

	// write a response to the client
	void write(SocketServer& server, Response&& response) override;

protected:
	// get a reference to the underlying client
	Client& client() const;
};

} // namespace vanity

#endif //VANITY_FAKE_CLIENT_H
