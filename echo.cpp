#include <iostream>

#include "socket_client.h"


/*
An EchoServer creates a server socket, accepts a connection,
echoes its first read and closes the connections
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

EchoServer::~EchoServer() = default;

void EchoServer::start(int port)
{
	m_socket.listen(port);
	std::cout << "Waiting for a connection" << std::endl;

	int epoll_s = epoll_create1(0);
	if (epoll_s < 0)
		throw vanity::SocketError("Could not create server epoll");
	epoll_event event_s{}, events_s[3];
	event_s.events = EPOLLIN;
	event_s.data.fd = m_socket.get_fd();
	int ctl_s = epoll_ctl(epoll_s, EPOLL_CTL_ADD, m_socket.get_fd(), &event_s);
	if (ctl_s < 0)
		throw vanity::SocketError("Could not add server to epoll");

	epoll_wait(epoll_s, events_s, 3, -1);
	vanity::SocketClient client{m_socket.accept()};
	std::cout << "Accepted a connection" << std::endl;

	int epoll = epoll_create1(0);
	if (epoll < 0)
		throw vanity::SocketError("Could not create epoll");

	client.register_event(epoll);
	epoll_event events[3];

	while (true){
		int n = epoll_wait(epoll, events, 3, -1);
		if (n < 0)
			throw vanity::SocketError("Could not wait for epoll");

		for (int i = 0; i < n; ++i){
			auto* ready_client = static_cast<vanity::SocketEventHandler*>(events[i].data.ptr);
			if (!ready_client->ready())
			{
				ready_client->unregister_event(epoll);
				return;
			}
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