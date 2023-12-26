//
// Created by kingsli on 11/28/23.
//

#ifndef VANITY_EXPIRY_DATABASE_SERVER_H
#define VANITY_EXPIRY_DATABASE_SERVER_H

#include "base_database_server.h"

namespace vanity {

/*
 * An ExpiryDatabaseServer allows us to respond to database requests for expiring keys
 */
class ExpiryDatabaseServer: public virtual BaseDatabaseServer
{
public:
	// a set_expiry request was received from a client
	void request_set_expiry(Client& client, const std::string& key, double seconds) override;

	// a get_expiry request was received from a client
	void request_get_expiry(Client& client, const std::string& key) override;

	// a clear_expiry request was received from a client
	void request_clear_expiry(Client& client, const std::string& key) override;

private:
	// convert a double in seconds to a std::chrono::time_point
	static std::chrono::time_point<std::chrono::system_clock> seconds_to_time_point(double seconds);

	// convert a std::chrono::time_point to a double in seconds
	static double time_point_to_seconds(std::chrono::time_point<std::chrono::system_clock> time_point);
};

} // namespace vanity

#endif //VANITY_EXPIRY_DATABASE_SERVER_H
