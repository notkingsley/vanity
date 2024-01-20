//
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_PIPE_SERVER_H
#define VANITY_PIPE_SERVER_H

#include "auth/locked_auth_server.h"
#include "socket/socket_server.h"

namespace vanity {

/*
 * A PipeServer is a server that handles PIPE requests
 */
class PipeServer : public virtual LockedAuthServer, public virtual SocketServer
{
public:
	// a pipe request was received from a client
	void request_pipe(Client& client, Request& request) override;

	// an exit request was received from a client
	void request_exit(Client& client) override;

	// a terminate request was received from a client
	void request_terminate(Client& client) override;
};

} // namespace vanity

#endif //VANITY_PIPE_SERVER_H
