//
// Created by kingsli on 11/3/23.
//

#include "set_database_server.h"
#include "serialize.h"


namespace vanity {

void SetDatabaseServer::request_set_add(Client &client, const std::string &key, std::unordered_set<std::string> values) {
	handle_result(client, database(client).set_add(key, std::move(values)));
}

void SetDatabaseServer::request_set_all(Client &client, const std::string &key) {
	handle_result(client, database(client).set_all(key));
}

void SetDatabaseServer::request_set_remove(Client &client, const std::string &key, int64_t count) {
	handle_result(client, database(client).set_remove(key, count));
}

void SetDatabaseServer::request_set_discard(Client &client, const std::string &key, std::unordered_set<std::string> values) {
	handle_result(client, database(client).set_discard(key, values));
}

void SetDatabaseServer::request_set_len(Client &client, const std::string &key) {
	handle_result(client, database(client).set_len(key));
}

void SetDatabaseServer::request_set_contains(Client &client, const std::string &key, const std::string &value) {
	handle_result(client, database(client).set_contains(key, value));
}

void SetDatabaseServer::request_set_move(Client &client, const std::string &source, const std::string &dest, const std::string &value) {
	handle_result(client, database(client).set_move(source, dest, value));
}

void SetDatabaseServer::request_set_union(Client &client, std::vector<std::string> keys) {
	handle_result(client, database(client).set_union(keys));
}

void SetDatabaseServer::request_set_union_into(Client &client, const std::string &dest, std::vector<std::string> keys) {
	handle_result(client, database(client).set_union_into(dest, keys));
}

void SetDatabaseServer::request_set_union_len(Client &client, std::vector<std::string> keys) {
	handle_result(client, database(client).set_union_len(keys));
}

void SetDatabaseServer::request_set_intersection(Client &client, std::vector<std::string> keys) {
	handle_result(client, database(client).set_intersection(keys));
}

void SetDatabaseServer::request_set_intersection_into(Client &client, const std::string &dest, std::vector<std::string> keys) {
	handle_result(client, database(client).set_intersection_into(dest, keys));
}

void SetDatabaseServer::request_set_intersection_len(Client &client, std::vector<std::string> keys) {
	handle_result(client, database(client).set_intersection_len(keys));
}

void SetDatabaseServer::request_set_difference(Client &client, const std::string &key1, const std::string &key2) {
	handle_result(client, database(client).set_difference(key1, key2));
}

void SetDatabaseServer::request_set_difference_into(Client &client, const std::string &dest, const std::string &key1, const std::string &key2) {
	handle_result(client, database(client).set_difference_into(dest, key1, key2));
}

void SetDatabaseServer::request_set_difference_len(Client &client, const std::string &key1, const std::string &key2) {
	handle_result(client, database(client).set_difference_len(key1, key2));
}

template<class T>
void SetDatabaseServer::handle_result(Client &client, const std::optional<T> &result) {
	if (result.has_value())
		send(client, ok(serialize(result.value())));
	else
		send(client, bad_type("not a set"));
}

} // namespace vanity