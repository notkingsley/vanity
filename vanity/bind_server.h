//
// Created by kingsli on 4/11/24.
//

#ifndef VANITY_BIND_SERVER_H
#define VANITY_BIND_SERVER_H

#include "request/request_server.h"
#include "socket/epoll_server.h"
#include "socket/socket_listener.h"
#include "utils/hash.h"


namespace vanity {

/*
 * A BindServer is a server that binds to a port and listens for incoming connections
 */
class BindServer : public virtual RequestServer, public virtual socket::EpollServer
{
private:
	using SocketListener = socket::SocketListener;

	using addr_type = std::pair<std::string, uint16_t>;

	// active SocketListeners
	std::unordered_map<
		addr_type,
		std::unique_ptr<SocketListener>,
		boost_hash::hash<std::string, uint16_t>
	> m_listeners;

	// the host to bind to
	std::string m_host;

	// all ports
	std::set<uint16_t> m_ports;

	// the port for the cluster
	uint16_t m_cluster_port;

public:
	explicit BindServer(std::string host, const std::vector<uint16_t>& ports, uint16_t cluster_port);

	// get the host and port of the peer socket
	// this returns the host and port of the last listener
	std::pair<std::string, uint16_t> get_peer_host_and_port() const;

	// a bind request was received from a client
	void request_bind(Client& client, const std::string& host, int64_t port) override;

	// an unbind request was received from a client
	void request_unbind(Client& client, const std::string& host, int64_t port) override;

protected:
	// bind and start listening on all ports
	void start();

	// the server is being exited. this deletes the socket listeners
	void stop();

private:
	// bind to a host and port
	// requires that the host and port are not already bound
	void bind(const std::string& host, uint16_t port);

	// validate that a port is in the valid range
	static bool validate_port(int64_t port) ;
};

} // namespace vanity

#endif //VANITY_BIND_SERVER_H
