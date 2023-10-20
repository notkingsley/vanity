//
// Created by kingsli on 10/9/23.
//

#include "session_server.h"

namespace vanity {

void SessionServer::request_switch_db(Client &client, int64_t db) {
	if (db < 0)
		throw InvalidRequest("db index must be non-negative");
	if (db >= M_DB_NUM)
		throw InvalidRequest("db index must be less than " + std::to_string(M_DB_NUM));

	client.set_db(db);
	send_ok(client);
}

} // namespace vanity