//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_SERVER_H
#define VANITY_BASE_DATABASE_SERVER_H

#include <array>

#include "db/db/locked_database.h"
#include "db/db/serial_database.h"
#include "request_server.h"
#include "logging.h"


namespace vanity {

//using db_type = db::SerialDatabase;
using db_type = db::LockedDatabase;

/*
 * A BaseDatabaseServer holds the common functionality of a DatabaseServer
 */
class BaseDatabaseServer : public virtual RequestServer
{
protected:
	// number of databases
	static constexpr size_t M_NUM_DATABASES = 16;

	// the databases
	std::array<db_type, M_NUM_DATABASES> m_databases;

public:
	// start the database threads
	void start();

	// stop the database threads
	void stop();

	// get the client's current selected database
	db_type& database(Client& client);

	// a get request was received from a client
	void request_get(Client& client, const std::string& key) override;

	// a del request was received from a client
	void request_del(Client& client, const std::string& key) override;

	// a type request was received from a client
	void request_type(Client& client, const std::string& key) override;

	// an exists request was received from a client
	void request_exists(Client& client, const std::string& key) override;

	// a reset request was received from a client
	void request_reset(Client& client) override;
};

} // namespace vanity

#endif //VANITY_BASE_DATABASE_SERVER_H
