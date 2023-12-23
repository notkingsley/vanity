//
// Created by kingsli on 9/15/23.
//

#ifndef VANITY_SERVER_H
#define VANITY_SERVER_H

#include "db/database_server.h"
#include "pipe_server.h"
#include "server_config.h"
#include "transaction_server.h"


namespace vanity {

/*
 * Top level server
 */
class Server:
	public virtual DatabaseServer,
	public virtual PipeServer,
	public virtual TransactionServer
{
private:
	// the configured port
	int m_port;

	// start background server tasks
	void start(){
		SocketServer::bind(m_port);
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
		Logger(config.log_file, config.log_level),
		PersistentServer(config.db_file),
		AuthServer(config.users_db),
		m_port(config.port) {};

	// run the server with the given configuration
	void run() {
		start();
		EventServer::run();
		stop();
	}
};

} // namespace vanity

#endif //VANITY_SERVER_H
