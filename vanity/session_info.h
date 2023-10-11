//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_INFO_H
#define VANITY_SESSION_INFO_H

#include <string>

namespace vanity {

// a client's authentication level
enum class client_auth
{
	UNKNOWN,
	USER,
	PEER, // and CLUSTER_MASTER and so on
	ADMIN,
};

/*
 * A SessionInfo is a struct that contains information about a client's session
 */
struct session_info
{
	// the client's username
	std::string username;

	// client auth
	client_auth auth = client_auth::UNKNOWN;

	// index of the database in use
	int database = 0;
};

} // namespace vanity

#endif //VANITY_SESSION_INFO_H
