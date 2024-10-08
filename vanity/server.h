//
// Created by kingsli on 9/15/23.
//

#ifndef VANITY_SERVER_H
#define VANITY_SERVER_H

#include "auth/locked_auth_server.h"
#include "cluster/cluster_server.h"
#include "cluster/peer_pulse_server.h"
#include "config.h"
#include "db/servers/database_server.h"
#include "db/transaction_server.h"
#include "persistent_server.h"
#include "pipe_server.h"
#include "poll_server.h"
#include "pubsub_server.h"


namespace vanity {

/*
 * Top level server
 */
class Server:
	public virtual ClusterServer,
	public virtual DatabaseServer,
	public virtual LockedAuthServer,
	public virtual PeerPulseServer,
	public virtual PersistentServer,
	public virtual PipeServer,
	public virtual PollServer,
	public virtual PubSubServer,
	public virtual TransactionServer
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
