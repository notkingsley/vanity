//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_INFO_H
#define VANITY_SESSION_INFO_H

#include <memory>
#include <set>
#include <string>
#include <variant>

namespace vanity {

// a client's authentication level
enum class client_auth
{
	UNKNOWN,
	USER,
	PEER, // and CLUSTER_MASTER and so on
	ADMIN,
};

// a client's current connection state
enum class conn_state
{
	NORMAL,
	PUBSUB,
	TRANSACTION,
};

// data on a currently running transaction
struct transaction_data
{
	// the commands in the transaction, in order
	std::string command;

	// the number of commands in the transaction
	int num_commands = 0;
};

// data on a currently running pubsub session
struct pubsub_data
{
	// the channels the client is subscribed to
	std::set<std::string> channels;
};

/*
 * A SessionInfo is a struct that contains information about a client's session
 */
struct session_info
{
	using conn_data_type = std::variant<transaction_data, pubsub_data>;

	// the client's username
	std::string username;

	// data on the current connection state
	std::unique_ptr<conn_data_type> conn_data;

	// client auth
	client_auth auth = client_auth::UNKNOWN;

	// the client's current connection state
	conn_state state = conn_state::NORMAL;

	// index of the database in use
	int database = 0;
};

} // namespace vanity

#endif //VANITY_SESSION_INFO_H
