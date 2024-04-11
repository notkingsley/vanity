#ifndef VANITY_SOCKET_SERVER_H
#define VANITY_SOCKET_SERVER_H

#include <unordered_set>
#include <memory>
#include <thread>

#include "bind_server.h"
#include "client/client_server.h"
#include "poll_server.h"


namespace vanity {

namespace socket {

/*
 * A SocketServer allows us to listen on one or more sockets,
 * accept connections and read/write data
 */
class SocketServer:
	public virtual BindServer,
	public virtual ClientServer,
	public virtual PollServer
{
public:
	// destroy the socket server
	~SocketServer() override = default;

protected:
	// start listening on all ports and start polling as a background task
	void start();

	// the server is being exited. this stops and joins the poll thread
	// and deletes the socket handlers (and the sockets themselves)
	void stop();
};

} // namespace socket

using socket::SocketServer;

} // namespace vanity

#endif // VANITY_SOCKET_SERVER_H