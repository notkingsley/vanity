//
// Created by kingsli on 11/8/23.
//

#include "hash_database_server.h"

namespace vanity {


void HashDatabaseServer::request_hash_set(Client &client, const std::string &key, std::unordered_map<std::string, std::string> values) {
	database(client).hash_set(key, std::move(values));
	send(client, ok());
}

void HashDatabaseServer::request_hash_all(Client &client, const std::string &key) {
	handle_result(client, database(client).hash_all(key));
}

void HashDatabaseServer::request_hash_get(Client &client, const std::string &key, const std::string &hash_key) {
	handle_result(client, database(client).hash_get(key, hash_key));
}

void HashDatabaseServer::request_hash_contains(Client &client, const std::string &key, const std::string &hash_key) {
	handle_result(client, database(client).hash_contains(key, hash_key));
}

void HashDatabaseServer::request_hash_len(Client &client, const std::string &key) {
	handle_result(client, database(client).hash_len(key));
}

void HashDatabaseServer::request_hash_key_len(Client &client, const std::string &key, const std::string &hash_key) {
	handle_result(client, database(client).hash_key_len(key, hash_key));
}

void HashDatabaseServer::request_hash_remove(Client &client, const std::string &key, const std::vector<std::string> &hash_keys) {
	handle_result(client, database(client).hash_remove(key, hash_keys));
}

void HashDatabaseServer::request_hash_keys(Client &client, const std::string &key) {
	handle_result(client, database(client).hash_keys(key));
}

void HashDatabaseServer::request_hash_values(Client &client, const std::string &key) {
	handle_result(client, database(client).hash_values(key));
}

void HashDatabaseServer::request_hash_update(Client &client, const std::string &key, std::unordered_map<std::string, std::string> values) {
	handle_result(client, database(client).hash_update(key, std::move(values)));
}

void HashDatabaseServer::request_hash_many_get(Client &client, const std::string &key, const std::vector<std::string> &hash_keys) {
	handle_result(client, database(client).hash_many_get(key, hash_keys));
}

template<class T>
void HashDatabaseServer::handle_result(Client &client, const std::variant<T, db::HashError> &result) {
	if (std::holds_alternative<db::HashError>(result))
		send_error(client, std::get<db::HashError>(result));
	else
		send(client, ok(std::get<T>(result)));
}

void HashDatabaseServer::send_error(Client &client, db::HashError err) {
	switch (err) {
		case db::HashError::NotHash:
		{
			send(client, bad_type("not hash"));
			break;
		}
		case db::HashError::BadKey:
		{
			send(client, error("bad key"));
			break;
		}
		default:
			throw std::runtime_error("invalid error kind");

	}
}

} // namespace vanity