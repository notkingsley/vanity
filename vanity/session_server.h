//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_SERVER_H
#define VANITY_SESSION_SERVER_H

#include "db/database_server.h"

namespace vanity {

/*
 * A SessionServer allows a client to update its session info
 */
class SessionServer : public virtual DatabaseServer
{
public:
	// a switch_db request was received from a client
	void request_switch_db(Client& client, int64_t db) override;
};

} // namespace vanity

#endif //VANITY_SESSION_SERVER_H
