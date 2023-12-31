//
// Created by kingsli on 10/8/23.
//

#ifndef VANITY_SOCKET_CLIENT_H
#define VANITY_SOCKET_CLIENT_H

#include "socket/socket_reader.h"
#include "socket/socket_writer.h"


namespace vanity {

/*
 * A SocketClient is a handle for an active connection,
 * and a handler for socket read events
 */
class SocketClient : public SocketReadHandler
{
protected:
	// the socket to read from
	ClientSocket m_socket;

	// the reader for the socket
	SocketReader m_reader;

	// the writer for the socket
	SocketWriter m_writer;

public:
	// create a client
	explicit SocketClient(ClientSocket&& socket)
		: m_socket(std::move(socket)), m_writer(m_socket) {};

	// move constructor
	SocketClient(SocketClient&& other) noexcept
		: m_socket(std::move(other.m_socket)), m_writer(m_socket) {};

	// move assignment
	SocketClient& operator=(SocketClient&& other) noexcept = delete;

	// get the socket file descriptor
	int socket_fd() const override { return m_socket.fd(); };

	// read from the socket
	size_t read(char* buffer, size_t buffer_size) const
	{
		return m_socket.read(buffer, buffer_size);
	}

	// write a message to the socket
	void write(SocketServer& server, std::string&& response)
	{
		m_writer.write(server, std::move(response));
	}
};

} // namespace vanity

#endif //VANITY_SOCKET_CLIENT_H
