//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_PROXY_CLIENT_H
#define VANITY_PROXY_CLIENT_H

#include "client.h"

namespace vanity {

/*
 * A ProxyClient implements the Client interface, while keeping
 * a reference to another Client&, forwarding all calls
 * as a default implementation
 */
class ProxyClient : public Client
{
private:
	// the actual client
	Client& m_client;

public:
	// create a ProxyClient with an underlying client
	explicit ProxyClient(Client& client);

	// the client has sent a message, and it is ready to be read
	void ready(SocketServer& server) override;

	// check if the client has permission to perform an op
	bool has_perm(operation_t op) const override;

	// return the client's session info
	struct session_info& session_info() override;

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

#endif //VANITY_PROXY_CLIENT_H
