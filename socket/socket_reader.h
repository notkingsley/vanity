#ifndef VANITY_SOCKET_READER_H
#define VANITY_SOCKET_READER_H

#include "abstract_base_server.h"
#include "socket.h"
#include "socket_event_handler.h"


namespace vanity{

/*
A SocketReader reads and buffers text from a ClientSocket,
emitting a message if the delimiter is encountered
*/
class SocketReader : public SocketReadHandler
{
private:
	// characters read at a time from the socket
	static const size_t m_chunk_size = 16;

	// the message delimiter
	static const char m_delimiter = '~';

	// buffer for message read
	std::string m_message;

	// the socket to read from
	ClientSocket m_socket;

	// if the delimiter was found at the end of the last read
	bool m_delimiter_read = false;

public:
	// create a SocketReader
	explicit SocketReader(ClientSocket&& socket);

	// Read once from the socket, buffering until the delimiter is found.
	// transparently alerts the server when a message is read
	// returns true if the socket is open, false if it is closed
	bool ready(AbstractBaseServer& server) override;

	// register to epoll for events
	void register_event(int epoll_fd) override;

	// unregister from epoll for events
	void unregister_event(int epoll_fd) override;
};

} // namespace vanity

#endif // VANITY_SOCKET_READER_H