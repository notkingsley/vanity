//
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_PIPE_SERVER_H
#define VANITY_PIPE_SERVER_H

#include "request_server.h"
#include "socket/socket_server.h"

namespace vanity {

/*
 * A PipeServer is a server that handles PIPE requests
 */
class PipeServer : public virtual RequestServer, public virtual SocketServer
{
public:
	// a pipe request was received from a client
	void request_pipe(Client& client, const std::string& msg, size_t& pos) override;
};

} // namespace vanity

#endif //VANITY_PIPE_SERVER_H
