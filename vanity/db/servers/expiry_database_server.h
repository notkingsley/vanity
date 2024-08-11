//
// Created by kingsli on 11/28/23.
//

#ifndef VANITY_EXPIRY_DATABASE_SERVER_H
#define VANITY_EXPIRY_DATABASE_SERVER_H

#include "base_database_server.h"
#include "repeat_event_server.h"
#include "utils/event.h"
#include "wal/wal_write_server.h"


namespace vanity {

/*
 * An ExpiryDatabaseServer allows us to respond to database requests for expiring keys
 */
class ExpiryDatabaseServer:
	public virtual BaseDatabaseServer,
	public virtual RepeatEventServer,
	public virtual wal::WalWriteServer
{
private:
	using time_point = std::chrono::time_point<std::chrono::system_clock>;

	// time between automatic emitted server_events in microseconds
	static constexpr long M_EXPIRE_INTERVAL = 10 * 1000 * 1000;

	// convert a double in seconds to a time_point
	static time_point seconds_to_time_point(double seconds);

	// convert a std::chrono::tp to a double in seconds
	static double time_point_to_seconds(time_point tp);

public:
	// create an ExpiryDatabaseServer
	ExpiryDatabaseServer();

	// an expire event was received
	void event_expire() override;

	// a set_expiry request was received from a client
	void request_set_expiry(Client& client, const std::string& key, double seconds) override;

	// a get_expiry request was received from a client
	void request_get_expiry(Client& client, const std::string& key) override;

	// a clear_expiry request was received from a client
	void request_clear_expiry(Client& client, const std::string& key) override;

	// redo a previously WALed set_expiry operation
	void wal_redo_set_expiry(const std::string& key, uint db, db::time_t expiry_time);
};

} // namespace vanity

#endif //VANITY_EXPIRY_DATABASE_SERVER_H
