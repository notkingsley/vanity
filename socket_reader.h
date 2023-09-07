#ifndef VANITY_SOCKET_READER_H
#define VANITY_SOCKET_READER_H

#include "base_server.h"
#include "socket.h"


namespace vanity{

/*
A SocketReader reads and buffers text from a ClientSocket,
returning a message if the delimiter is encountered
*/
class SocketReader
{
private:
	// characters read at a time from the socket
	static const size_t m_chunk_size = 16;

	// the message delimiter
	const std::string m_delimiter{"~~~~"};

	// temporary buffer of unparsed characters or partial delimiter
	// we copy here after a message or part of delimiter is hit,
	// and use it directly as buffer on next read() to avoid copying
	std::string m_buffer;

	// buffer for message read
	std::string m_message;

	// the socket to read from
	ClientSocket& m_socket;

public:
	// create a SocketReader
	explicit SocketReader(ClientSocket& socket);

	// Read once from the socket. this will block if the socket is empty.
	// this reads exactly once from the socket, even if the data fills the buffer,
	// this is to avoid blocking in the case that the message exactly fills the buffer
	// returns a pair of (is_open, message)
	// if is_open is false, socket is closed and empty and message will be empty
	// if is_open is true, message will be returned if a delimiter was found,
	// empty otherwise
	std::pair<bool, std::string> read();
};

} // namespace vanity

#endif // VANITY_SOCKET_READER_H