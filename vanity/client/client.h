//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_CLIENT_H
#define VANITY_CLIENT_H

#include "operations.h"
#include "response/response.h"
#include "write_manager.h"


namespace vanity {

/*
 * A Client is, well, a client
 *
 * This is merely an interface for a client,
 * the methods are implemented by various types of clients
 */
class Client
{
public:
	// check if the client has permission to perform an op
	virtual bool has_perm(operation_t op) const = 0;

	// return the client's session info
	virtual struct session_info& session_info() = 0;

	// request to close the client, not effective immediately
	// but when the client makes a request
	virtual void close() = 0;

	// read from the socket
	virtual size_t read(char* buffer, size_t buffer_size) const = 0;

	// write a response to the client
	virtual void write(WriteManager& manager, Response&& response) = 0;
};

} // namespace vanity

#endif // VANITY_CLIENT_H
