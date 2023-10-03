//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_DATABASE_SERVER_H
#define VANITY_DATABASE_SERVER_H


#include "database.h"
#include "instruction_server.h"
#include "logging.h"


namespace vanity {

/*
 * A DatabaseServer allows us to respond to database requests
 */
class DatabaseServer : public virtual InstructionServer, protected virtual Logger
{
protected:
	// the database
	db::Database m_database;

public:
	// a get request was received from a client
	void instruction_get(const ClientSocket& socket, const std::string& key) override;

	// a set request was received from a client
	void instruction_set(const ClientSocket& socket, const std::string& key, const std::string& value) override;

	// a set request was received from a client
	void instruction_set(const ClientSocket& socket, const std::string& key, const int64_t& value) override;

	// a set request was received from a client
	void instruction_set(const ClientSocket& socket, const std::string& key, const double& value) override;

	// a del request was received from a client
	void instruction_del(const ClientSocket& socket, const std::string& key) override;

	// a wipe request was received from a client
	void instruction_reset(const ClientSocket& socket) override;
};

} // namespace vanity

#endif //VANITY_DATABASE_SERVER_H
