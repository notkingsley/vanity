//
// Created by kingsli on 4/8/24.
//

#ifndef VANITY_CLIENT_SERVER_H
#define VANITY_CLIENT_SERVER_H

#include <unordered_set>

#include "abstract_server.h"
#include "client/tcp_client.h"
#include "socket/epoll_server.h"


namespace vanity {

/*
 * A ClientServer holds and manages all clients
 */
class ClientServer:
	public virtual AbstractServer,
	public virtual ClientManager,
	public virtual socket::EpollServer
{
private:
	// the current set of clients
	std::unordered_set<std::unique_ptr<TcpClient>> m_clients;

	// cast this to a ClientManager
	ClientManager& as_client_manager();

	// cast this to a ReadManager
	ReadManager& as_read_manager();

	// a SocketReadHandler is ready
	void read_handler_ready(SocketReadHandler *handler) override;

public:
	// add a new client
	void add_client(TcpClient&& client) override;

	// remove a client
	void remove_client(TcpClient& client) override;

	// get a callback for when a message is received
	handle_callback_t handle_callback(TcpClient& client) override;

	// send a message to a client
	void send(Client& client, Response&& response) override;

protected:
	// stop the server
	// this will remove all clients
	void stop();
};

} // namespace vanity

#endif //VANITY_CLIENT_SERVER_H
