#ifndef VANITY_SOCKET_H
#define VANITY_SOCKET_H

#include <exception>
#include <string>
#include <netdb.h>


namespace vanity{

/*
There was a connection related error
*/
class SocketError : public std::exception
{
private:
	std::string m_msg;

public:
	SocketError(const std::string& msg) : m_msg(msg) {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

/*
A socket holding a connection
*/
class Socket
{
protected:
	// the file descriptor of the socket
	int m_fd;
	sockaddr_in m_addr;
	socklen_t m_addr_size = sizeof(m_addr);

	// can't be instantiated directly
	Socket() {};

public:
	// close the connection
	virtual ~Socket();

	// no copy
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
};

/*
A class representing a socket connection that has been received
*/
class ClientSocket : Socket
{
public:
	// instantiate a ClientSocket object,
	// blocks until a connection is received
	ClientSocket(int server_fd);

	// read a string from the socket
	size_t read(char* buffer, size_t buffer_size);

	// write a string to the socket
	void write(std::string msg);
};

/*
A server socket binds and listens for connections
*/
class ServerSocket : Socket
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