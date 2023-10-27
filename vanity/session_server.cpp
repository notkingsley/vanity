//
// Created by kingsli on 10/9/23.
//

#include "session_server.h"

namespace vanity {

void SessionServer::request_switch_db(Client &client, int64_t db) {
	if (db < 0)
		return send(client, error(" db index must be non-negative"));

	if (db >= M_NUM_DATABASES)
		return send(client, error(" db index must be less than " + std::to_string(M_NUM_DATABASES)));

	client.db(db);
	send(client, ok());
}

} // namespace vanity