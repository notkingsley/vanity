//
// Created by kingsli on 10/29/23.
//

#include "primitive_database_server.h"

namespace vanity {

void PrimitiveDatabaseServer::request_set_str(Client &client, const std::string &key, const std::string &value) {
	database(client).set_str(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_set_int(Client &client, const std::string &key, const int64_t &value) {
	database(client).set_int(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_set_float(Client &client, const std::string &key, const double &value) {
	database(client).set_float(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_incr_int(Client &client, const std::string &key, db::int_t value) {
	auto result = database(client).incr_int(key, value);

	if (result.has_value())
		send(client, ok(result.value()));
	else
		send(client, bad_type());
}

void PrimitiveDatabaseServer::request_incr_float(Client &client, const std::string &key, db::float_t value) {
	auto result = database(client).incr_float(key, value);

	if (result.has_value())
		send(client, ok(result.value()));
	else
		send(client, bad_type());
}

void PrimitiveDatabaseServer::request_str_len(Client &client, const std::string &key) {
	auto result = database(client).str_len(key);

	if (result.has_value())
		send(client, ok(result.value()));
	else
		send(client, bad_type());
}

void PrimitiveDatabaseServer::request_many_get(Client &client, const std::vector<std::string> &keys) {
	auto result = database(client).many_get(keys);
	send(client, ok(result));
}

} // namespace vanity