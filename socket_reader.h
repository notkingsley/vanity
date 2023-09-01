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
	const std::string m_delimiter{"$$$"};

	// temporary buffer of unparsed characters or partial delimiter
	// we copy here after a message is hit, and copy back on next read()
	// this is only expensive if there's a lot of characters
	// present in the buffer after the delimiter when we read()
	std::string m_buffer;

	// buffer for message read
	std::string m_message;

	// the socket to read from
	ClientSocket& m_socket;

	// return true if c is to be ignored in the stream
	bool ignores(char c) { return c == '\n' or c == '\r'; }

public:
	// create a SocketReader
	SocketReader(ClientSocket& socket);

	// Read once from the socket. this will block if the socket is empty.
	// this reads exactly once from the socket, even if the data fills the buffer,
	// this is to avoid blocking in the case that the message exactly fills the buffer
	// returns a pair of (is_open, message)
	// if is_open is false, socket is closed and message will be empty
	// if is_open is true, message will be returned if a delimiter was found,
	// empty otherwise
	std::pair<bool, std::string> read();
};

} // namespace vanity

#endif // VANITY_SOCKET_READER_H