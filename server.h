//
// Created by kingsli on 9/15/23.
//

#ifndef VANITY_SERVER_H
#define VANITY_SERVER_H

#include "database_server.h"
#include "instruction_server.h"
#include "socket/socket_server.h"


namespace vanity {

/*
 * Top Level server
 */
class Server:
	public DatabaseServer,
	public SocketServer
{ };

} // namespace vanity

#endif //VANITY_SERVER_H
