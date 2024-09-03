//
// Created by kingsli on 9/3/24.
//

#ifndef VANITY_GENERAL_DATABASE_SERVER_H
#define VANITY_GENERAL_DATABASE_SERVER_H

#include "base_database_server.h"

namespace vanity {

/*
 * A GeneralDatabaseServer handles general database operations
 */
class GeneralDatabaseServer : public virtual BaseDatabaseServer
{
public:
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

#endif //VANITY_GENERAL_DATABASE_SERVER_H
