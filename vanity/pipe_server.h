//B
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_PIPE_SERVER_H
#define VANITY_PIPE_SERVER_H

#include "client/write_manager.h"
#include "client/exit_server.h"
#include "request/request_server.h"


namespace vanity {

/*
 * A PipeServer is a server that handles PIPE requests
 */
class PipeServer:
	public virtual ExitServer,
	public virtual RequestServer,
	public virtual WriteManager
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
