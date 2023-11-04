//
// Created by kingsli on 11/3/23.
//

#ifndef VANITY_SET_DATABASE_SERVER_H
#define VANITY_SET_DATABASE_SERVER_H

#include "base_database_server.h"

namespace vanity {

/*
 * A SetDatabaseServer is a database server that handles set operations
 */
class SetDatabaseServer : public virtual BaseDatabaseServer
{
public:
	// a set_add request was received from a client
	void request_set_add(Client& client, const std::string& key, std::unordered_set<std::string> values) override;

	// a set_all request was received from a client
	void request_set_all(Client& client, const std::string& key) override;

	// a set_remove request was received from a client
	void request_set_remove(Client& client, const std::string& key, int64_t count) override;

	// a set_discard request was received from a client
	void request_set_discard(Client& client, const std::string& key, std::unordered_set<std::string> values) override;

	// a set_len request was received from a client
	void request_set_len(Client& client, const std::string& key) override;

	// a set_contains request was received from a client
	void request_set_contains(Client& client, const std::string& key, const std::string& value) override;

	// a set_move request was received from a client
	void request_set_move(Client& client, const std::string& source, const std::string& dest, const std::string& value) override;

	// a set_union request was received from a client
	void request_set_union(Client& client, std::vector<std::string> keys) override;

	// a set_union_into request was received from a client
	void request_set_union_into(Client& client, const std::string& dest, std::vector<std::string> keys) override;

	// a set_union_len request was received from a client
	void request_set_union_len(Client& client, std::vector<std::string> keys) override;

	// a set_intersection request was received from a client
	void request_set_intersection(Client& client, std::vector<std::string> keys) override;

	// a set_intersection_into request was received from a client
	void request_set_intersection_into(Client& client, const std::string& dest, std::vector<std::string> keys) override;

	// a set_intersection_len request was received from a client
	void request_set_intersection_len(Client& client, std::vector<std::string> keys) override;

	// a set_difference request was received from a client
	void request_set_difference(Client& client, const std::string& key1, const std::string& key2) override;

	// a set_difference_into request was received from a client
	void request_set_difference_into(Client& client, const std::string& dest, const std::string& key1, const std::string& key2) override;

	// a set_difference_len request was received from a client
	void request_set_difference_len(Client& client, const std::string& key1, const std::string& key2) override;

private:
	// handle an optional result returned by a set request
	template<class T>
	void handle_result(Client& client, const std::optional<T>& result);
};

} // namespace vanity


#endif //VANITY_SET_DATABASE_SERVER_H
