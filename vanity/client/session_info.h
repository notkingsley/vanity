//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_INFO_H
#define VANITY_SESSION_INFO_H

#include <memory>
#include <set>
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

const std::initializer_list<std::pair<client_auth, std::string>> CLIENT_AUTH_STRINGS {
	{client_auth::UNKNOWN, "UNKNOWN"},
	{client_auth::USER,    "USER"},
	{client_auth::ADMIN,   "ADMIN"},
	{client_auth::PEER,    "PEER"},
};

// a client's current connection state
enum class conn_state
{
	NORMAL,
	TRANSACTION,
};

// data on a currently running transaction
struct transaction_data
{
	// the commands in the transaction, in order
	std::string commands;

	// the number of commands in the transaction
	size_t size = 0;

	// push a command to the transaction and increment the number of commands
	template<class T>
	void push(T& command) {
		commands += command;
		++size;
	}
};

template<conn_state state>
struct conn_data {};

template<>
struct conn_data<conn_state::TRANSACTION> {
	using type = transaction_data;
};

template<conn_state state>
using conn_data_t = typename conn_data<state>::type;

/*
 * A SessionInfo is a struct that contains information about a client's session
 */
struct session_info
{
	// the pubsub channels the client is subscribed to
	std::set<std::string> channels;

	// the client's username
	std::string username;

	// data on the current connection state
	// we'll use a variant when we have more than one state
	std::unique_ptr<transaction_data> conn_data;

	// client auth
	client_auth auth = client_auth::UNKNOWN;

	// the client's current connection state
	conn_state state = conn_state::NORMAL;

	// index of the database in use
	int database = 0;
};

} // namespace vanity

#endif //VANITY_SESSION_INFO_H
