//
// Created by kingsli on 11/28/23.
//

#include "expiry_database_server.h"

namespace vanity {

ExpiryDatabaseServer::ExpiryDatabaseServer() {
	repeat(server_event::expire, M_EXPIRE_INTERVAL);
}

void ExpiryDatabaseServer::event_expire() {
	for (auto& db : m_databases)
		db.deep_purge();
}

void ExpiryDatabaseServer::request_set_expiry(Client &client, const std::string &key, double seconds) {
	auto expiry_time = seconds_to_time_point(seconds);
	wal_set_expiry(key, session_db(client), expiry_time);
	database(client).set_expiry(key, expiry_time);
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

void ExpiryDatabaseServer::wal_redo_set_expiry(const std::string &key, uint db, db::time_t expiry_time) {
	m_databases[db].set_expiry(key, expiry_time);
}

} // namespace vanity