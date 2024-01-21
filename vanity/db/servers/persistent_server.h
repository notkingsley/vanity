//
// Created by kingsli on 10/3/23.
//

#ifndef VANITY_PERSISTENT_SERVER_H
#define VANITY_PERSISTENT_SERVER_H

#include <filesystem>
#include <optional>

#include "base_database_server.h"
#include "repeat_event_server.h"
#include "utils/event.h"


namespace vanity {

/*
 * A database that handles persistence to disk
 */
class PersistentServer:
	public virtual BaseDatabaseServer,
	public virtual RepeatEventServer
{
private:
	// time between automatic persistence calls in microseconds
	static constexpr long M_PERSIST_INTERVAL = 10 * 60 * 1000 * 1000;

	// file to persist the database to, if any
	const std::optional<std::filesystem::path> m_db_file;

public:
	// create a Persistent Database Server with an optional persistence file
	explicit PersistentServer(std::optional<std::filesystem::path> db_file) noexcept;

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
