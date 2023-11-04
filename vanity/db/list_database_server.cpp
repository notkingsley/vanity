//
// Created by kingsli on 10/29/23.
//

#include "list_database_server.h"

namespace vanity {

void ListDatabaseServer::request_list_len(Client &client, const std::string &key) {
	handle_result(client, database(client).list_len(key));
}

void ListDatabaseServer::request_list_get(Client &client, const std::string &key, int64_t index) {
	handle_result(client, database(client).list_get(key, index));
}

void ListDatabaseServer::request_list_set(Client &client, const std::string &key, int64_t index, const std::string &value) {
	auto result = database(client).list_set(key, index, value);
	if (std::holds_alternative<db::ListErrorKind>(result))
		send_error(client, std::get<db::ListErrorKind>(result));
	else
		send(client, ok());
}

void ListDatabaseServer::request_list_push_left(Client &client, const std::string &key, std::list<std::string> values) {
	handle_result(client, database(client).list_push_left(key, std::move(values)));
}

void ListDatabaseServer::request_list_push_right(Client &client, const std::string &key, std::list<std::string> values) {
	handle_result(client, database(client).list_push_right(key, std::move(values)));
}

void ListDatabaseServer::request_list_pop_left(Client &client, const std::string &key, int64_t count) {
	handle_result(client, database(client).list_pop_left(key, count));
}

void ListDatabaseServer::request_list_pop_right(Client &client, const std::string &key, int64_t count) {
	handle_result(client, database(client).list_pop_right(key, count));
}

void ListDatabaseServer::request_list_range(Client &client, const std::string &key, int64_t start, int64_t stop) {
	handle_result(client, database(client).list_range(key, start, stop));
}

void ListDatabaseServer::request_list_trim(Client &client, const std::string &key, int64_t start, int64_t stop) {
	handle_result(client, database(client).list_trim(key, start, stop));
}

void ListDatabaseServer::request_list_remove(Client &client, const std::string &key, const std::string &value, int64_t count) {
	handle_result(client, database(client).list_remove(key, value, count));
}

template<class T>
void ListDatabaseServer::handle_result(Client &client, const std::variant<T, db::ListErrorKind> &result) {
	if (std::holds_alternative<db::ListErrorKind>(result))
		send_error(client, std::get<db::ListErrorKind>(result));
	else
		send(client, ok(std::get<T>(result)));
}

void ListDatabaseServer::send_error(Client &client, db::ListErrorKind kind) {
	switch (kind) {
		case db::ListErrorKind::NotList:
		{
			return send(client, bad_type("not a list"));
		}
		case db::ListErrorKind::OutOfRange:
		{
			return send(client, error("index out of range"));
		}
		default:
			throw std::runtime_error("invalid error kind");
	}
}

} // namespace vanity