#ifndef VANITY_SOCKET_H
#define VANITY_SOCKET_H

#include <exception>
#include <cerrno>
#include <string>
#include <utility>
#include <netdb.h>
#include <sys/epoll.h>

#include "exceptions.h"


namespace vanity::socket {

/*
 * A socket holding a connection
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
 * A class representing a socket connection that has been received
 * This class is used to read and write to the socket
*/
class Socket : public BaseSocket
{
public:
	// instantiate a Socket object,
	// blocks until a connection is received
	explicit Socket(int server_fd);

	// read a string from the socket
	size_t read(char* buffer, size_t buffer_size) const;

	// write from a buffer to the socket
	size_t write(const char* buffer, size_t buffer_size) const;
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
	void listen(int port);
};

} // namespace vanity::socket

#endif // VANITY_SOCKET_H