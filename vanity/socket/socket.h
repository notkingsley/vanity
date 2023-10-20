#ifndef VANITY_SOCKET_H
#define VANITY_SOCKET_H

#include <exception>
#include <cerrno>
#include <string>
#include <utility>
#include <netdb.h>
#include <sys/epoll.h>

#include "exceptions.h"


namespace vanity{

// forward declaration
class SocketEventHandler;

/*
A socket holding a connection
*/
class Socket
{
protected:
	// the file descriptor of the socket
	int m_fd{};
	sockaddr_in m_addr{};
	socklen_t m_addr_size = sizeof(m_addr);

	// can't be instantiated directly
	Socket() = default;

public:
	// close the connection
	virtual ~Socket();

	// no copy
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	// move constructor
	Socket(Socket&& other) noexcept;
	Socket& operator=(Socket&& other) noexcept;

	// register to epoll for events with handler
	void register_event(int epoll_fd, SocketEventHandler& handler) const;

	// unregister from epoll for events
	void unregister_event(int epoll_fd) const;
};

/*
A class representing a socket connection that has been received
*/
class ClientSocket : public Socket
{
public:
	// instantiate a ClientSocket object,
	// blocks until a connection is received
	explicit ClientSocket(int server_fd);

	// read a string from the socket
	size_t read(char* buffer, size_t buffer_size) const;

	// write a string to the socket
	void write(const std::string& msg) const;

	// write from a buffer to the socket
	size_t write(const char* buffer, size_t buffer_size) const;
};

/*
A server socket binds and listens for connections
*/
class ServerSocket : public Socket
{
public:
	ServerSocket();

	// accept a connection
	ClientSocket accept();

	// start listening for connections
	void listen(int port);
};

} // namespace vanity

#endif // VANITY_SOCKET_H