//
// Created by kingsli on 9/15/23.
//

#ifndef VANITY_SERVER_H
#define VANITY_SERVER_H

#include "db/servers/database_server.h"
#include "pipe_server.h"
#include "pubsub_server.h"
#include "server_config.h"
#include "transaction_server.h"


namespace vanity {

/*
 * Top level server
 */
class Server:
	public virtual DatabaseServer,
	public virtual PipeServer,
	public virtual PubSubServer,
	public virtual TransactionServer
{
private:
	// start background server tasks
	void start(){
		SocketServer::start();
		SerialAuthServer::start();
		DatabaseServer::start();

		logger().info("Started server");
	}

	// stop any running tasks and exit the server
	void stop(){
		DatabaseServer::stop();
		SerialAuthServer::stop();
		SocketServer::stop();

		logger().info("Stopped server");
	}

public:
	// create a server
	explicit Server(const ServerConfig& config):
		AuthServer(config.users_db),
		Logger(config.log_file, config.log_level),
		PersistentServer(config.db_file),
		SocketServer(config.ports) {};

	// run the server with the given configuration
	void run() {
		Server::start();
		EventServer::run();
		Server::stop();
	}
};

} // namespace vanity

#endif //VANITY_SERVER_H
