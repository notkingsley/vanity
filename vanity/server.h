//
// Created by kingsli on 9/15/23.
//

#ifndef VANITY_SERVER_H
#define VANITY_SERVER_H

#include "config.h"
#include "db/servers/database_server.h"
#include "pipe_server.h"
#include "pubsub_server.h"
#include "transaction_server.h"
#include "wal/wal_server.h"


namespace vanity {

/*
 * Top level server
 */
class Server:
	public virtual DatabaseServer,
	public virtual PipeServer,
	public virtual PubSubServer,
	public virtual TransactionServer,
	public virtual WalServer
{
private:
	// lock file
	std::optional<std::filesystem::path> m_lock_file;

	// start background server tasks
	void start();

	// stop any running tasks and exit the server
	void stop();

public:
	// create a server
	explicit Server(const Config& config);

	// run the server with the given configuration
	void run();
};

} // namespace vanity

#endif //VANITY_SERVER_H
