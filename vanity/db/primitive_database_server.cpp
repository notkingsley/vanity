//
// Created by kingsli on 10/29/23.
//

#include "primitive_database_server.h"

namespace vanity {

void PrimitiveDatabaseServer::request_get(Client &client, const std::string &key) {
	auto value = database(client).get(key);
	if (not value.has_value())
		return send(client, null());

	auto data = value.value();
	switch (data.index()) {
		case 0:
			return send(client, ok(serialize(std::get<0>(data))));
		case 1:
			return send(client, ok(serialize(std::get<1>(data))));
		case 2:
			return send(client, ok(serialize(std::get<2>(data))));
		default:
			throw std::runtime_error("invalid type");
	}
}

void PrimitiveDatabaseServer::request_set(Client &client, const std::string &key, const std::string &value) {
	database(client).set(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_set(Client &client, const std::string &key, const int64_t &value) {
	database(client).set(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_set(Client &client, const std::string &key, const double &value) {
	database(client).set(key, value);
	send(client, ok());
}

void PrimitiveDatabaseServer::request_incr_int(Client &client, const std::string &key, db::int_t value) {
	auto result = database(client).incr_int(key, value);

	if (result.has_value())
		send(client, ok(serialize(result.value())));
	else
		send(client, bad_type());
}

void PrimitiveDatabaseServer::request_incr_float(Client &client, const std::string &key, db::float_t value) {
	auto result = database(client).incr_float(key, value);

	if (result.has_value())
		send(client, ok(serialize(result.value())));
	else
		send(client, bad_type());
}

void PrimitiveDatabaseServer::request_len_str(Client &client, const std::string &key) {
	auto result = database(client).len_str(key);

	if (result.has_value())
		send(client, ok(serialize(result.value())));
	else
		send(client, bad_type());
}

void PrimitiveDatabaseServer::request_many_get(Client &client, const std::vector<std::string> &keys) {
	auto result = database(client).many_get(keys);
	send(client, ok(serialize(result)));
}

} // namespace vanity