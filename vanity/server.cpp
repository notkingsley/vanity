//
// Created by kingsli on 3/14/24.
//

#include "file_lock.h"
#include "server.h"

namespace vanity {

Server::Server(const Config &config):
	AuthServer(config.auth_file),
	BindServer(config.host, config.ports, config.cluster_port),
	LogServer(config.log_file, config.log_level),
	PersistJournalServer(config.wal_file, config.db_file, config.journal_file),
	m_lock_file{config.lock_file}
{
	PersistJournalServer::recover();
}

void Server::start() {
	BindServer::start();
	PollServer::start();
	RepeatEventServer::start();
	logger().info("Started server");
}

void Server::stop() {
	RepeatEventServer::stop();
	PersistentServer::stop();
	ClientServer::stop();
	PollServer::stop();
	BindServer::stop();
	logger().info("Stopped server");
}

void Server::run() {
	FileLock lock {m_lock_file};

	Server::start();
	EventServer::run();
	Server::stop();
}

} // namespace vanity
