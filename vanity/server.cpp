//
// Created by kingsli on 3/14/24.
//

#include "server.h"

namespace vanity {

Server::Server(const Config &config):
	AuthServer(config.users_db),
	LogServer(config.log_file, config.log_level),
	SocketServer(config.ports),
	PersistJournalServer(config.wal_file, config.db_file, config.journal_file)
{
	WalServer::recover();
}

void Server::start() {
	SocketServer::start();
	DatabaseServer::start();
	RepeatEventServer::start();
	logger().info("Started server");
}

void Server::stop() {
	RepeatEventServer::stop();
	DatabaseServer::stop();
	SocketServer::stop();
	logger().info("Stopped server");
}

void Server::run() {
	Server::start();
	EventServer::run();
	Server::stop();
}

} // namespace vanity