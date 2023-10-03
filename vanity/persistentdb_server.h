//
// Created by kingsli on 10/3/23.
//

#ifndef VANITY_PERSISTENTDB_SERVER_H
#define VANITY_PERSISTENTDB_SERVER_H

#include <filesystem>
#include <optional>

#include "database_server.h"


namespace vanity {

/*
 * A database that handles persistence to disk
 */
class PersistentDBServer : public virtual DatabaseServer{
private:
	// file to persist the database to, if any
	const std::optional<std::filesystem::path> m_db_file;

public:
	explicit PersistentDBServer(std::optional<std::filesystem::path> db_file) noexcept;

	// persist the database to the file
	bool persist() const;

	// a persist request was received from a client
	void instruction_persist(const ClientSocket& socket) override;
};

} // namespace vanity

#endif //VANITY_PERSISTENTDB_SERVER_H
