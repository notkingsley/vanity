//
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_CONCRETE_CLIENT_H
#define VANITY_CONCRETE_CLIENT_H

#include "client.h"
#include "socket/socket_reader.h"
#include "socket/socket_writer.h"


namespace vanity {

/*
 * A ConcreteClient is a client that has been
 * accepted by a SocketServer
 * This is the actual implementation of a Client
 */
class ConcreteClient : public Client, public socket::SocketReadHandler
{
private:
	using Socket = socket::Socket;
	using SocketReader = socket::SocketReader;
	using SocketWriter = socket::SocketWriter;

	// the client's session info
	struct session_info m_session_info;

	// the writer for the socket
	SocketWriter m_writer;

	// the reader for the socket
	SocketReader m_reader;

	// the socket to read from
	Socket m_socket;

public:
	// create a client
	explicit ConcreteClient(Socket&& socket);

	// move constructor
	ConcreteClient(ConcreteClient&&) noexcept;

	// move assignment
	ConcreteClient& operator=(ConcreteClient&&) noexcept = delete;

	// the client has sent a message, and it is ready to be read
	void ready(SocketServer& server) override;

	// check if the client has permission to perform an op
	bool has_perm(operation_t op) const override;

	// return the client's session info
	struct session_info& session_info() override;

	// write a response to the client
	void write(WriteManager& manager, Response&& response) override;

	// get the socket file descriptor
	int socket_fd() const override;
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
