//
// Created by kingsli on 10/3/23.
//

#ifndef VANITY_PERSISTENT_SERVER_H
#define VANITY_PERSISTENT_SERVER_H

#include <filesystem>
#include <optional>

#include "log_server.h"
#include "repeat_event_server.h"
#include "request/request_handler.h"
#include "response/response_server.h"
#include "db/wal/persist_journal_server.h"


namespace vanity {

/*
 * A database that handles persistence to disk
 */
class PersistentServer:
	public virtual LogServer,
	public virtual PersistJournalServer,
	public virtual RepeatEventServer,
	public virtual RequestHandler,
	public virtual ResponseServer
{
private:
	// time between automatic persistence calls in microseconds
	static constexpr long M_PERSIST_INTERVAL = 10 * 60 * 1000 * 1000;

public:
	// create a Persistent Database Server with an optional persistence file
	explicit PersistentServer() noexcept;

	// persist the database to the file
	bool persist();

	// stop auto persistence, persisting one last time
	void stop();

	// a persist request was received from a client
	void request_persist(Client& client) override;

	// a persist event was received
	void event_persist() override;
};

} // namespace vanity

#endif //VANITY_PERSISTENT_SERVER_H
