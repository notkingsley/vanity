//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_DATABASE_SERVER_H
#define VANITY_DATABASE_SERVER_H


#include <filesystem>
#include <optional>

#include "database.h"
#include "instruction_server.h"


namespace vanity {

/*
 * A DatabaseServer allows us to respond to database requests
 */
class DatabaseServer : public virtual InstructionServer
{
private:
	// the database
	StringDatabase m_database;

	// file to persist the database to, if any
	const std::optional<std::filesystem::path> m_db_file;

public:
	// create a database server
	explicit DatabaseServer(std::optional<std::filesystem::path> db_file) noexcept;

	// a get request was received from a client
	void instruction_get(const ClientSocket& socket, const std::string& key) override;

	// a set request was received from a client
	void instruction_set(const ClientSocket& socket, const std::string& key, const std::string& value) override;

	// a del request was received from a client
	void instruction_del(const ClientSocket& socket, const std::string& key) override;

	// a persist request was received from a client
	void instruction_persist(const ClientSocket& socket) override;
};

} // namespace vanity

#endif //VANITY_DATABASE_SERVER_H
