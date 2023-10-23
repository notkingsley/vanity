//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_DATABASE_SERVER_H
#define VANITY_DATABASE_SERVER_H

#include <array>

#include "serial_database.h"
#include "request_server.h"
#include "logging.h"


namespace vanity {

/*
 * A DatabaseServer allows us to respond to database requests
 */
class DatabaseServer : public virtual RequestServer, protected virtual Logger
{
protected:
	// number of databases
	static constexpr size_t M_NUM_DATABASES = 16;

	// the databases
	std::array<db::SerialDatabase, M_NUM_DATABASES> m_databases;

public:
	// start the database threads
	void start();

	// stop the database threads
	void stop();

	// get the client's current selected database
	db::SerialDatabase& database(Client& client);

	// a get request was received from a client
	void request_get(Client& client, const std::string& key) override;

	// a set request was received from a client
	void request_set(Client& client, const std::string& key, const std::string& value) override;

	// a set request was received from a client
	void request_set(Client& client, const std::string& key, const int64_t& value) override;

	// a set request was received from a client
	void request_set(Client& client, const std::string& key, const double& value) override;

	// a del request was received from a client
	void request_del(Client& client, const std::string& key) override;

	// a type request was received from a client
	void request_type(Client& client, const std::string& key) override;

	// an exists request was received from a client
	void request_exists(Client& client, const std::string& key) override;

	// an incr_int request was received from a client
	void request_incr_int(Client& client, const std::string& key, db::int_t value) override;

	// an incr_float request was received from a client
	void request_incr_float(Client& client, const std::string& key, db::float_t value) override;

	// a reset request was received from a client
	void request_reset(Client& client) override;
};

} // namespace vanity

#endif //VANITY_DATABASE_SERVER_H
