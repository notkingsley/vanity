//
// Created by kingsli on 10/29/23.
//

#include "primitive_database_server.h"

namespace vanity {

void PrimitiveDatabaseServer::request_str_set(Client &client, const std::string &key, std::string value) {
	database(client).str_set(key, std::move(value));
	send(client, ok());
}

void PrimitiveDatabaseServer::request_int_set(Client &client, const std::string &key, int64_t value) {
	database(client).int_set(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_float_set(Client &client, const std::string &key, double value) {
	database(client).float_set(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_incr_int(Client &client, const std::string &key, db::int_t value) {
	auto result = database(client).incr_int(key, value);

	if (result.has_value())
		send(client, ok(result.value()));
	else
		send(client, bad_type("not an int"));
}

void PrimitiveDatabaseServer::request_incr_float(Client &client, const std::string &key, db::float_t value) {
	auto result = database(client).incr_float(key, value);

	if (result.has_value())
		send(client, ok(result.value()));
	else
		send(client, bad_type("not a float"));
}

void PrimitiveDatabaseServer::request_str_len(Client &client, const std::string &key) {
	auto result = database(client).str_len(key);

	if (result.has_value())
		send(client, ok(result.value()));
	else
		send(client, bad_type("not a string"));
}

void PrimitiveDatabaseServer::request_many_get(Client &client, const std::vector<std::string> &keys) {
	auto result = database(client).many_get(keys);
	send(client, ok(result));
}

} // namespace vanity