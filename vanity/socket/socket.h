#ifndef VANITY_SOCKET_H
#define VANITY_SOCKET_H

#include <cerrno>
#include <string>
#include <netdb.h>


namespace vanity::socket {

/*
 * A socket of some kind
*/
class BaseSocket
{
protected:
	// the file descriptor of the socket
	int m_fd{};

	// can't be instantiated directly
	BaseSocket() = default;

public:
	// close the connection
	~BaseSocket();

	// no copy
	BaseSocket(const BaseSocket&) = delete;
	BaseSocket& operator=(const BaseSocket&) = delete;

	// move constructor
	BaseSocket(BaseSocket&& other) noexcept;
	BaseSocket& operator=(BaseSocket&& other) noexcept;

	// get the socket's file descriptor
	int fd() const;
};

/*
 * A socket connection that has been received
 * This class is used to read and write to the socket
*/
class Socket : public BaseSocket
{
public:
	// create a socket from a file descriptor
	explicit Socket(int fd);

	// read a string from the socket
	size_t read(char* buffer, size_t buffer_size) const;

	// write from a buffer to the socket
	size_t write(const char* buffer, size_t buffer_size) const;

	static Socket connect(const char* host, uint16_t port);
};

/*
 * A server socket binds and listens for connections
*/
class ServerSocket : public BaseSocket
{
public:
	ServerSocket();

	// accept a connection
	Socket accept();

	// start listening for connections
	void listen(const char* host, uint16_t port);

	// return the host and port of the server
	std::pair<std::string, uint16_t> get_host_and_port() const;
};

} // namespace vanity::socket

#endif // VANITY_SOCKET_H
