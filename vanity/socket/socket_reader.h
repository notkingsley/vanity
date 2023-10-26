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
emitting a message when one is read
*/
class SocketReader
{
private:
	// buffer for message read
	std::string m_message;

	// the remaining size of the current message, if any (0 if no message)
	size_t m_message_left = 0;

public:
	// Read from the client's socket, buffering until a message is read
	// transparently calls server.handle() when a message is read
	// returns true if the socket is open, false if it is closed
	bool read(AbstractServer& server, Client& client);
};

} // namespace vanity

#endif // VANITY_SOCKET_READER_H