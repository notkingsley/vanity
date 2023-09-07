#ifndef VANITY_SOCKET_CLIENT_H
#define VANITY_SOCKET_CLIENT_H


#include "socket_event_handler.h"
#include "socket_reader.h"


namespace vanity{

/*
A SocketClient represent an actively connected entity on the
other end of a socket connection
*/
class SocketClient : public SocketReadHandler
{
private:
	ClientSocket m_socket;
	SocketReader m_reader;

public:
	// create a socket client
	explicit SocketClient(ClientSocket&& socket);

	// register to epoll for events
	void register_event(int epoll_fd) override;

	// unregister from epoll for events
	void unregister_event(int epoll_fd) override;

	// the socket is ready for reading
	// return true if the socket should be kept, false otherwise
	bool ready(AbstractBaseServer& server) override;
};

} // namespace vanity

#endif // VANITY_SOCKET_CLIENT_H