//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_SERVER_H
#define VANITY_SESSION_SERVER_H

#include "request_server.h"

namespace vanity {

/*
 * A SessionServer allows a client to update its session info
 */
class SessionServer : public virtual RequestServer
{
public:
	// the number of databases (doesn't belong here)
	static constexpr int M_DB_NUM = 16;

	// a switch_db request was received from a client
	void request_switch_db(const Client& client, int64_t db) override;
};

} // namespace vanity

#endif //VANITY_SESSION_SERVER_H
