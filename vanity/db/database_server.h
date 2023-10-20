//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_DATABASE_SERVER_H
#define VANITY_DATABASE_SERVER_H


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
	// the database
	db::SerialDatabase m_database;

public:
	// start the database threads
	void start();

	// stop the database threads
	void stop();

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

	// a wipe request was received from a client
	void request_reset(Client& client) override;
};

} // namespace vanity

#endif //VANITY_DATABASE_SERVER_H
