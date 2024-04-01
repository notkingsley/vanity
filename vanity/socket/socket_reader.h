#ifndef VANITY_SOCKET_READER_H
#define VANITY_SOCKET_READER_H

#include <functional>

#include "socket.h"


namespace vanity::socket {

/*
A SocketReader reads and buffers text from a Socket,
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
	using callback_t = std::function<void(const std::string&)>;

	// Read from the client's socket, buffering until a message is read
	// transparently calls the callback with the message
	// returns true if the socket is open, false if it is closed
	bool read(Socket& socket, const callback_t& callback);
};

} // namespace vanity::socket

#endif // VANITY_SOCKET_READER_H
