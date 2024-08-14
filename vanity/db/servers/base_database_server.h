//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_SERVER_H
#define VANITY_BASE_DATABASE_SERVER_H

#include "database_object_server.h"
#include "request/request_server.h"


namespace vanity {

/*
 * A BaseDatabaseServer holds the common functionality of a DatabaseServer
 */
class BaseDatabaseServer : public virtual DatabaseObjectServer, public virtual RequestServer
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

#endif //VANITY_BASE_DATABASE_SERVER_H
