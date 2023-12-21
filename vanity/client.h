//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_CLIENT_H
#define VANITY_CLIENT_H

#include "permissions.h"
#include "socket/socket_client.h"

namespace vanity {

/*
 * A Client is, well, a client
 *
 * This is merely an interface for a client,
 * the methods are implemented by ConcreteClient
 */
class Client
{
public:
	// the client has sent a message, and it is ready to be read
	virtual void ready(SocketServer& server) = 0;

	// check if the client has permission to perform an op
	virtual bool has_perm(operation_t op) const = 0;

	// set the client's active database index
	virtual void db(int db) = 0;

	// get the client's active database index
	virtual int db() const = 0;

	// update the client's auth level
	virtual void set_auth(const client_auth& auth) = 0;

	// set the client's username
	virtual void username(const std::string& username) = 0;

	// get the client's username
	virtual const std::string& username() const = 0;

	// get the client's current connection state
	virtual conn_state state() const = 0;

	// set the client's current connection state
	virtual void state(conn_state state) = 0;

	// get a reference to the client's current transaction data
	// if the client is not in a transaction, this will throw
	virtual transaction_data& get_transaction_data() = 0;

	// request to close the client, not effective immediately
	// but when the client makes a request
	virtual void close() = 0;

	// read from the socket
	virtual size_t read(char* buffer, size_t buffer_size) const = 0;

	// write a response to the client
	virtual void write(SocketServer& server, Response&& response) = 0;
};

} // namespace vanity
#endif // VANITY_CLIENT_H
