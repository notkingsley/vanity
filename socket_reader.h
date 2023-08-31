#ifndef VANITY_SOCKET_READER_H
#define VANITY_SOCKET_READER_H

#include "base_server.h"
#include "socket.h"


namespace vanity{

/*
A SocketReader reads and buffers text from a ClientSocket,
emitting a message once a message delimiter is encountered
*/
class SocketReader
{
private:
	// characters read at a time from the socket
	static const size_t m_chunk_size = 1024;

	// the message delimiter
	const std::string m_delimiter{"$$$"};

	// characters read from the delimiter during last read
	size_t m_delimiter_read = 0;

	// the buffer
	std::string m_buffer;

	// the socket to read from
	ClientSocket& m_socket;

	// the server to emit messages to
	BaseServer& m_server;

	// emit a message that has been read
	void emit(std::string msg) { m_server.handle(msg, m_socket); }

	// return true if c is to be ignored in the stream
	bool ignores(char c) { return c == '\n' or c == '\r'; }

public:
	// create a SocketReader
	SocketReader(BaseServer& server, ClientSocket& socket);

	// Read once from the socket. Emits a message when delimiter is encountered
	// this will block if the socket is empty.
	// this reads exactly once from the socket, even if the data fills the buffer,
	// this is to avoid blocking in the case that the message exactly fills the buffer
	// returns false if socket is closed, else true
	bool read();
};

} // namespace vanity

#endif // VANITY_SOCKET_READER_H