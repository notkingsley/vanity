//
// Created by kingsli on 11/8/23.
//

#ifndef VANITY_HASH_DATABASE_SERVER_H
#define VANITY_HASH_DATABASE_SERVER_H

#include "base_database_server.h"

namespace vanity{

/*
 * A HashDatabaseServer handles hash operations
 */
class HashDatabaseServer : public virtual BaseDatabaseServer
{
public:
	// a hash_set request was received from a client
	void request_hash_set(Client& client, const std::string& key, std::unordered_map<std::string, std::string> values) override;

	// a hash_all request was received from a client
	void request_hash_all(Client& client, const std::string& key) override;

	// a hash_get request was received from a client
	void request_hash_get(Client& client, const std::string& key, const std::string& hash_key) override;

	// a hash_contains request was received from a client
	void request_hash_contains(Client& client, const std::string& key, const std::string& hash_key) override;

	// a hash_len request was received from a client
	void request_hash_len(Client& client, const std::string& key) override;

	// a hash_key_len request was received from a client
	void request_hash_key_len(Client& client, const std::string& key, const std::string& hash_key) override;

	// a hash_remove request was received from a client
	void request_hash_remove(Client& client, const std::string& key, const std::vector<std::string>& hash_keys) override;

	// a hash_keys request was received from a client
	void request_hash_keys(Client& client, const std::string& key) override;

	// a hash_values request was received from a client
	void request_hash_values(Client& client, const std::string& key) override;

	// a hash_update request was received from a client
	void request_hash_update(Client& client, const std::string& key, std::unordered_map<std::string, std::string> values) override;

	// a hash_many_get request was received from a client
	void request_hash_many_get(Client& client, const std::string& key, const std::vector<std::string>& hash_keys) override;

private:
	// handle a variant result returned by a hash request
	template<class T>
	void handle_result(Client& client, const std::variant<T, db::HashError>& result);

	// send a db::HashError to the client
	void send_error(Client& client, db::HashError kind);
};

} // namespace vanity

#endif //VANITY_HASH_DATABASE_SERVER_H
