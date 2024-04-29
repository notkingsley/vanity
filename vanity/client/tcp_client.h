//
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_TCP_CLIENT_H
#define VANITY_TCP_CLIENT_H

#include "client.h"
#include "socket/client_read_handler.h"
#include "socket/socket_reader.h"
#include "socket/socket_writer.h"


namespace vanity {

/*
 * A TcpClient is a client that's connected via TCP
 */
class TcpClient : public Client, public socket::ClientReadHandler
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
	explicit TcpClient(Socket&& socket);

	// move constructor
	TcpClient(TcpClient&&) noexcept;

	// move assignment
	TcpClient& operator=(TcpClient&&) noexcept = delete;

	// the client has sent a message, and it is ready to be read
	void ready(ClientManager& manager) override;

	// check if the client has permission to perform an op
	bool has_perm(operation_t op) const override;

	// return the client's session info
	struct session_info& session_info() override;

	// write a sendable to the client
	void write(WriteManager& manager, Sendable&& sendable) override;

	// get the socket file descriptor
	int socket_fd() const override;
};

bool operator==(const TcpClient& lhs, const TcpClient& rhs);

} // namespace vanity

namespace std {

template<>
struct hash<vanity::TcpClient> : public hash<const vanity::TcpClient*> {
	size_t operator()(const vanity::TcpClient &c) const {
		return hash<const vanity::TcpClient*>::operator()(&c);
	}
};

} // namespace std

#endif //VANITY_TCP_CLIENT_H
