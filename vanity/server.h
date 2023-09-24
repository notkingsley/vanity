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
 * Configuration for the server
 */
struct ServerConfig
{
	std::optional<std::filesystem::path> db_file;
	int port;
};

/*
 * Top Level server
 */
class Server:
	public DatabaseServer,
	public SocketServer
{
private:
	// the configuration
	ServerConfig m_config;

	// listen for incoming connections
	void listen(){
		SocketServer::listen(m_config.port);
	};

public:
	// create a server
	explicit Server(const ServerConfig& config) noexcept :
		DatabaseServer(config.db_file),
		m_config(config) {};

	// run the server with the given configuration
	void run(){
		listen();
		start();
	};
};

} // namespace vanity

#endif //VANITY_SERVER_H
