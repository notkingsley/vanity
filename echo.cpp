#include <iostream>
#include <sys/socket.h>

#include "socket_reader.h"


/*
An EchoServer creates a server socket, accepts a connection,
echoes it's first read and closes the connections
*/
class EchoServer
{
private:
	// the socket that listens for connections
	vanity::ServerSocket m_socket;

public:
	// create an echo server
	EchoServer();
	~EchoServer();

	// start the server
	void start(int port);
};

EchoServer::EchoServer() : m_socket{} {}

EchoServer::~EchoServer() {}

void EchoServer::start(int port)
{
	m_socket.listen(port);
	std::cout << "Waiting for a connection" << std::endl;
	vanity::ClientSocket client = m_socket.accept();
	std::cout << "Accepted a connection" << std::endl;

	vanity::BaseServer server{};
	vanity::SocketReader reader{server, client};

	while (true){
		if (reader.read()){
			std::cout << "Read data\n";
		} else {
			std::cout << "Connection closed\n";
			break;
		}
	}
}

int main()
{
	try{
		EchoServer server{};
		server.start(9955);
	}
	catch (vanity::SocketError& e){
		std::cout << e.what() << std::endl;
	}
	return 0;
}