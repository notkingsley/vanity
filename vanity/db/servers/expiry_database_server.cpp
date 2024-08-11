//
// Created by kingsli on 11/28/23.
//

#include "expiry_database_server.h"

namespace vanity {

ExpiryDatabaseServer::ExpiryDatabaseServer() {
	repeat(server_event::expire, M_EXPIRE_INTERVAL);
}

void ExpiryDatabaseServer::event_expire() {
	deep_purge_databases();
}

void ExpiryDatabaseServer::request_set_expiry(Client &client, const std::string &key, double seconds) {
	database(client).set_expiry(key, seconds_to_time_point(seconds));
	send(client, ok());
}

void ExpiryDatabaseServer::request_get_expiry(Client &client, const std::string &key) {
	if (auto expiry_time = database(client).get_expiry(key))
		send(client, ok(time_point_to_seconds(*expiry_time)));
	else
		send(client, null());
}

void ExpiryDatabaseServer::request_clear_expiry(Client &client, const std::string &key) {
	database(client).clear_expiry(key);
	send(client, ok());
}

auto ExpiryDatabaseServer::seconds_to_time_point(double seconds) -> time_point {
	using namespace std::chrono;
	return system_clock::now() + duration_cast<system_clock::duration>(duration<double>(seconds));
}

double ExpiryDatabaseServer::time_point_to_seconds(time_point tp) {
	using namespace std::chrono;
	return duration_cast<duration<double>>(tp - system_clock::now()).count();
}

} // namespace vanity