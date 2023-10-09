//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_INFO_H
#define VANITY_SESSION_INFO_H

namespace vanity {

// a client's authentication level
enum class client_auth
{
	UNKNOWN,
	USER,
	PEER,
	ADMIN,
};

/*
 * A SessionInfo is a struct that contains information about a client's session
 */
struct session_info
{
	// client auth
	client_auth auth = client_auth::ADMIN;

	// index of the database in use
	int database = 0;
};

} // namespace vanity

#endif //VANITY_SESSION_INFO_H
