//
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_CONCRETE_CLIENT_H
#define VANITY_CONCRETE_CLIENT_H

#include "client.h"
#include "socket/socket_client.h"


namespace vanity {

/*
 * A ConcreteClient is a client that has been
 * accepted by a SocketServer
 * This is the actual implementation of a Client
 */
class ConcreteClient : public Client, public SocketClient
{
private:
	// the client's session info
	struct session_info m_session_info;

	// if this client has been requested to be closed
	bool m_closed = false;

public:
	// create a client
	explicit ConcreteClient(ClientSocket&& socket);

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
	void write(WriteManager& manager, Response&& response) override;
};

bool operator==(const ConcreteClient& lhs, const ConcreteClient& rhs);

} // namespace vanity

namespace std {

template<>
struct hash<vanity::ConcreteClient> : public hash<const vanity::ConcreteClient*> {
	size_t operator()(const vanity::ConcreteClient &c) const {
		return hash<const vanity::ConcreteClient*>::operator()(&c);
	}
};

} // namespace std

#endif //VANITY_CONCRETE_CLIENT_H
