//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_SERVER_H
#define VANITY_BASE_DATABASE_SERVER_H

#include <array>

#include "db/db/locked_database.h"
#include "db/db/serial_database.h"
#include "request/request_server.h"


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

	// a switch_db request was received from a client
	void request_switch_db(Client& client, int64_t db) override;

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

	// a keys request was received from a client
	void request_keys(Client& client) override;

	// a copy_to request was received from a client
	void request_copy_to(Client& client, const std::string& from, const std::string& to) override;

	// a move_to request was received from a client
	void request_move_to(Client& client, const std::string& from, const std::string& to) override;

	// a copy_to_db request was received from a client
	void request_copy_to_db(Client& client, const std::string& from, int64_t dest) override;

	// a move_to_db request was received from a client
	void request_move_to_db(Client& client, const std::string& from, int64_t dest) override;

private:
	// validate a db index or send an error to the client
	// return true if the index was valid, false otherwise
	bool validate_db_index(Client& client, int64_t index);
};

} // namespace vanity

#endif //VANITY_BASE_DATABASE_SERVER_H
