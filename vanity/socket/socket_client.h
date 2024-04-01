//
// Created by kingsli on 10/8/23.
//

#ifndef VANITY_SOCKET_CLIENT_H
#define VANITY_SOCKET_CLIENT_H

#include "socket/socket_reader.h"
#include "socket/socket_writer.h"


namespace vanity {

namespace socket {

/*
 * A SocketClient is a handle for an active connection,
 * and a handler for socket read events
 */
class SocketClient : public SocketReadHandler
{
protected:
	// the socket to read from
	Socket m_socket;

	// the reader for the socket
	SocketReader m_reader;

	// the writer for the socket
	SocketWriter m_writer;

public:
	// create a client
	explicit SocketClient(Socket&& socket);

	// move constructor
	SocketClient(SocketClient&& other) noexcept;

	// move assignment
	SocketClient& operator=(SocketClient&& other) noexcept = delete;

	// get the socket file descriptor
	int socket_fd() const override;

	// write a message to the socket
	void write(WriteManager& manager, std::string&& response);
};

} // namespace socket

using socket::SocketClient;
using socket::Socket;

} // namespace vanity

#endif //VANITY_SOCKET_CLIENT_H
