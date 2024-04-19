//
// Created by kingsli on 4/11/24.
//

#ifndef VANITY_BIND_SERVER_H
#define VANITY_BIND_SERVER_H

#include <vector>

#include "socket/epoll_server.h"
#include "socket/socket_listener.h"


namespace vanity {

/*
 * A BindServer is a server that binds to a port and listens for incoming connections
 */
class BindServer : public virtual socket::EpollServer
{
private:
	// active SocketListeners
	std::vector<socket::SocketListener> m_listeners;

	// the host to bind to
	std::string m_host;

	// all ports
	std::vector<uint16_t> m_ports;

	// the port for the cluster
	uint16_t m_cluster_port;

public:
	explicit BindServer(std::string host, std::vector<uint16_t> ports, uint16_t cluster_port);

	// get the host and port of the peer socket
	// this returns the host and port of the last listener
	std::pair<std::string, uint16_t> get_peer_host_and_port() const;

protected:
	// bind and start listening on all ports
	void start();

	// the server is being exited. this deletes the socket listeners
	void stop();
};

} // namespace vanity

#endif //VANITY_BIND_SERVER_H
