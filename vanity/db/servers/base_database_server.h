//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_SERVER_H
#define VANITY_BASE_DATABASE_SERVER_H

#include <array>

#include "db/db/locked_database.h"
#include "request/request_server.h"


namespace vanity {

/*
 * A BaseDatabaseServer holds the common functionality of a DatabaseServer
 */
class BaseDatabaseServer : public virtual RequestServer
{
private:
	// the type of database
	using db_type = db::LockedDatabase;

	// the type of logger
	using logger_type = db_type::logger_type;

protected:
	// number of databases
	static constexpr size_t M_NUM_DATABASES = 16;

	// the databases
	std::array<db_type, M_NUM_DATABASES> m_databases;

	// get the client's current selected database
	db_type& database(Client& client);

	// get the database at the given index
	db_type& database(size_t index);

	// convenience function to deep_purge() all databases
	void deep_purge_databases();

	// enable (or disable) expiry for all databases
	void enable_databases_expiry(bool enable);

public:
	// create a new BaseDatabaseServer
	explicit BaseDatabaseServer(logger_type& logger);

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

	// create and return an array of databases
	template <std::size_t... I>
	static constexpr std::array<db_type, sizeof...(I)> create_databases_helper(std::index_sequence<I...>, logger_type& logger) {
		return { db_type(I, logger)... };
	}

	// create and return an array of databases
	static std::array<db_type, M_NUM_DATABASES> create_databases(logger_type& logger) {
		return create_databases_helper(std::make_index_sequence<M_NUM_DATABASES>{}, logger);
	}
};

} // namespace vanity

#endif //VANITY_BASE_DATABASE_SERVER_H
