#ifndef VANITY_SOCKET_READER_H
#define VANITY_SOCKET_READER_H

#include "socket.h"

namespace vanity{

// forward declaration
class Client;

// forward declaration
class AbstractServer;

/*
A SocketReader reads and buffers text from a ClientSocket,
emitting a message if the delimiter is encountered
*/
class SocketReader
{
private:
	// characters read at a time from the socket
	static const size_t m_chunk_size = 16;

	// the message delimiter
	static const char m_delimiter = '~';

	// buffer for message read
	std::string m_message;

	// if the delimiter was found at the end of the last read
	bool m_delimiter_read = false;

public:
	// Read once from the client's socket, buffering until the delimiter is found.
	// transparently alerts the server when a message is read
	// returns true if the socket is open, false if it is closed
	bool read(AbstractServer& server, Client& client);
};

} // namespace vanity

#endif // VANITY_SOCKET_READER_H