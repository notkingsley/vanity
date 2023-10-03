//
// Created by kingsli on 9/15/23.
//

#ifndef VANITY_SERVER_H
#define VANITY_SERVER_H

#include "persistentdb_server.h"
#include "instruction_server.h"
#include "socket/socket_server.h"


namespace vanity {

/*
 * Configuration for the server
 */
struct ServerConfig
{
	std::optional<std::filesystem::path> db_file;
	std::filesystem::path log_file;
	LogLevel log_level;
	int port;
};

/*
 * Top level server
 */
class Server:
	public PersistentDBServer,
	public SocketServer
{
private:
	// the configuration
	ServerConfig m_config;

	// start background server tasks
	void start(){
		SocketServer::bind(m_config.port);
		SocketServer::start();
		logger().info("Started server");
	}

	// stop any running tasks and exit the server
	void stop(){
		PersistentDBServer::persist();
		SocketServer::stop();
		logger().info("Stopped server");
	}

public:
	// create a server
	explicit Server(const ServerConfig& config) noexcept :
		Logger(config.log_file, config.log_level),
		PersistentDBServer(config.db_file),
		m_config(config) {};

	// run the server with the given configuration
	void run(){
		start();
		try{
			while (true) {
				switch (m_event_queue.get()) {
					case server_event::socket_ready: {
						SocketServer::socket_ready();
						break;
					}
					case server_event::persist: {
						PersistentDBServer::persist();
						break;
					}
				}
			}
		}
		catch (DestroyServer&) {
			stop();
		}
	};
};

} // namespace vanity

#endif //VANITY_SERVER_H
