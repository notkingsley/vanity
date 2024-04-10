//
// Created by kingsli on 3/14/24.
//

#include "file_lock.h"
#include "server.h"

namespace vanity {

Server::Server(const Config &config):
	AuthServer(config.auth_file),
	LogServer(config.log_file, config.log_level),
	PersistJournalServer(config.wal_file, config.db_file, config.journal_file),
	BindServer(config.ports),
	m_lock_file{config.lock_file}
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
	FileLock lock {m_lock_file};

	Server::start();
	EventServer::run();
	Server::stop();
}

} // namespace vanity
