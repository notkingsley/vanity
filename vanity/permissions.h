//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_PERMISSIONS_H
#define VANITY_PERMISSIONS_H

#include "session_info.h"

namespace vanity {

// all the possible operations
enum class operation_t{
	GET,
	SET,
	DEL,
	SWITCH_DB,
	TYPE,
	EXISTS,
	INCR_INT,
	INCR_FLOAT,
	LEN_STR,
	MANY_GET,
	ADD_USER,
	EDIT_USER,
	DEL_USER,
	AUTH,
	CHANGE_PASSWORD,
	PERSIST,
	EXIT,
	TERMINATE,
	RESET,
	PING,
};

// check if an operation is permitted for an unknown client
inline bool unknown_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::AUTH:
		case operation_t::PING:
		case operation_t::EXIT:
			return true;
		case operation_t::GET:
		case operation_t::SET:
		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::LEN_STR:
		case operation_t::MANY_GET:
		case operation_t::SWITCH_DB:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::PERSIST:
		case operation_t::TERMINATE:
		case operation_t::RESET:
		default:
			return false;
	}
}

// check if an operation is permitted for a user
inline bool user_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::GET:
		case operation_t::SET:
		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::LEN_STR:
		case operation_t::MANY_GET:
		case operation_t::SWITCH_DB:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::PERSIST:
		case operation_t::EXIT:
		case operation_t::PING:
			return true;
		case operation_t::AUTH:
		case operation_t::TERMINATE:
		case operation_t::RESET:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		default:
			return false;
	}
}

// check if an operation is permitted for a peer
inline bool peer_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::GET:
		case operation_t::SET:
		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::LEN_STR:
		case operation_t::MANY_GET:
		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:
		case operation_t::EXIT:
		case operation_t::PING:
		case operation_t::TERMINATE:
		case operation_t::RESET:
			return true;
		case operation_t::AUTH:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		default:
			return false;
	}
}

// check if an operation is permitted for an admin
inline bool admin_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::GET:
		case operation_t::SET:
		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::LEN_STR:
		case operation_t::MANY_GET:
		case operation_t::SWITCH_DB:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::PERSIST:
		case operation_t::EXIT:
		case operation_t::PING:
		case operation_t::TERMINATE:
		case operation_t::RESET:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
			return true;
		case operation_t::AUTH:
		default:
			return false;
	}
}

// check if an op is permitted for a given client_auth
inline bool is_permitted(operation_t op, client_auth auth)
{
	switch (auth)
	{
		case client_auth::UNKNOWN:
			return unknown_is_permitted(op);
		case client_auth::USER:
			return user_is_permitted(op);
		case client_auth::PEER:
			return peer_is_permitted(op);
		case client_auth::ADMIN:
			return admin_is_permitted(op);
		default:
			return false;
	}
}

} // namespace vanity

#endif //VANITY_PERMISSIONS_H
