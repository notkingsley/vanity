//
// Created by kingsli on 10/3/23.
//

#ifndef VANITY_PERSISTENT_SERVER_H
#define VANITY_PERSISTENT_SERVER_H

#include <filesystem>
#include <optional>

#include "base_database_server.h"
#include "event_server.h"
#include "event.h"


namespace vanity {

/*
 * A database that handles persistence to disk
 */
class PersistentServer : public virtual BaseDatabaseServer, public virtual EventServer
{
private:
	// time between automatic persistence calls in microseconds
	static constexpr long M_PERSIST_INTERVAL = 10 * 60 * 1000 * 1000;

	// file to persist the database to, if any
	const std::optional<std::filesystem::path> m_db_file;

	// whether the auto persistence has been stopped
	event m_stopped;

	// the persist_loop thread
	std::thread m_persist_loop_thread;

public:
	// create a Persistent Database Server with an optional persistence file
	explicit PersistentServer(std::optional<std::filesystem::path> db_file) noexcept;

	// persist the database to the file
	bool persist();

	// start auto persistence as a background task
	void start();

	// stop auto persistence, persisting one last time
	void stop();

	// a persist request was received from a client
	void request_persist(Client& client) override;

	// a persist event was received
	void event_persist() override;

private:
	// run in a separate thread, yielding server_event::persist
	// to the event queue every M_PERSIST_INTERVAL microseconds
	void persist_loop();
};

} // namespace vanity

#endif //VANITY_PERSISTENT_SERVER_H
