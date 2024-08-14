//
// Created by kingsli on 8/14/24.
//

#ifndef VANITY_DATABASE_OBJECT_SERVER_H
#define VANITY_DATABASE_OBJECT_SERVER_H

#include <array>

#include "db/db/locked_database.h"
#include "session_server.h"


namespace vanity {

/*
 * A DatabaseObjectServer holds the database objects and
 * performs the common functionalities of other DatabaseServers
 */
class DatabaseObjectServer : public virtual SessionServer
{
private:
	// the type of logger
	using wal_logger_t = wal::WriteAheadLogger;

	// the logger
	wal_logger_t m_wal_logger;

	// the type of database
	using db_type = db::LockedDatabase;

protected:
	// number of databases
	static constexpr size_t M_NUM_DATABASES = 16;

	// the databases
	std::array<db_type, M_NUM_DATABASES> m_databases;

	// get a reference to the wal logger
	wal_logger_t& wal_logger();

	// get the client's current selected database
	db_type& database(Client& client);

	// get the database at the given index
	db_type& database(size_t index);

	// convenience function to deep_purge() all databases
	void deep_purge_databases();

	// enable (or disable) expiry for all databases
	void enable_databases_expiry(bool enable);

private:
	// create and return an array of databases
	template <std::size_t... I>
	static constexpr std::array<db_type, sizeof...(I)> create_databases_helper(std::index_sequence<I...>, wal_logger_t& logger) {
		return { db_type(I, logger)... };
	}

	// create and return an array of databases
	std::array<db_type, M_NUM_DATABASES> create_databases() {
		return create_databases_helper(std::make_index_sequence<M_NUM_DATABASES>{}, m_wal_logger);
	}

public:
	// create a new DatabaseObjectServer
	DatabaseObjectServer();
};


} // namespace vanity

#endif //VANITY_DATABASE_OBJECT_SERVER_H
