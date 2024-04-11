//
// Created by kingsli on 9/7/23.
//

#include "socket_server.h"

namespace vanity::socket {

void SocketServer::start() {
	BindServer::start();
	PollServer::start();
}

void SocketServer::stop() {
	PollServer::stop();
	ClientServer::stop();
	BindServer::stop();
}

} // namespace vanity::socket
