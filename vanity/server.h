//
// Created by kingsli on 9/15/23.
//

#ifndef VANITY_SERVER_H
#define VANITY_SERVER_H

#include "auth/serial_auth_server.h"
#include "db/persistent_server.h"
#include "pipe_server.h"
#include "signals.h"
#include "session_server.h"
#include "socket/socket_server.h"


namespace vanity {

/*
 * Configuration for the server
 */
struct ServerConfig
{
	std::optional<std::filesystem::path> users_db;
	std::optional<std::filesystem::path> db_file;
	std::filesystem::path log_file;
	LogLevel log_level;
	int port;
};

/*
 * Top level server
 */
class Server:
	public virtual PersistentServer,
	public virtual PipeServer,
	public virtual SerialAuthServer,
	public virtual SessionServer,
	public virtual SocketServer
{
private:
	// the configuration
	ServerConfig m_config;

	// start background server tasks
	void start(){
		signal::set_server(this);

		SocketServer::bind(m_config.port);
		SocketServer::start();
		SerialAuthServer::start();
		DatabaseServer::start();
		PersistentServer::start();

		logger().info("Started server");
	}

	// stop any running tasks and exit the server
	void stop(){
		PersistentServer::stop();
		DatabaseServer::stop();
		SerialAuthServer::stop();
		SocketServer::stop();

		logger().info("Stopped server");
	}

public:
	// create a server
	explicit Server(const ServerConfig& config) noexcept :
		Logger(config.log_file, config.log_level),
		PersistentServer(config.db_file),
		AuthServer(config.users_db),
		m_config(config) {};

	// request to terminate the server
	void terminate() override {
		m_event_queue.push(server_event::terminate);
	}

	// run the server with the given configuration
	void run(){
		start();

		while (true) {
			switch (m_event_queue.get()) {
				case server_event::socket_ready: {
					SocketServer::socket_ready();
					break;
				}
				case server_event::persist: {
					PersistentServer::persist();
					break;
				}
				case server_event::terminate:
					return stop();
			}
		}
	}
};

} // namespace vanity

#endif //VANITY_SERVER_H
