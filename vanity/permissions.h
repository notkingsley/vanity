//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_PERMISSIONS_H
#define VANITY_PERMISSIONS_H

#include "session_info.h"

namespace vanity {

// all the possible operations
enum class operation_t{
	EXIT,
	TERMINATE,
	PING,
	PIPE,

	ADD_USER,
	EDIT_USER,
	DEL_USER,
	AUTH,
	CHANGE_PASSWORD,

	SWITCH_DB,
	PERSIST,

	TRANSACT_BEGIN,
	TRANSACT_COMMIT,
	TRANSACT_DISCARD,

	DEL,
	TYPE,
	EXISTS,
	RESET,
	KEYS,
	COPY_TO,
	MOVE_TO,
	COPY_TO_DB,
	MOVE_TO_DB,

	SET_EXPIRY,
	GET_EXPIRY,
	CLEAR_EXPIRY,

	GET,
	STR_SET,
	INT_SET,
	FLOAT_SET,
	INCR_INT,
	INCR_FLOAT,
	STR_LEN,
	MANY_GET,

	LIST_LEN,
	LIST_GET,
	LIST_SET,
	LIST_PUSH_LEFT,
	LIST_PUSH_RIGHT,
	LIST_POP_LEFT,
	LIST_POP_RIGHT,
	LIST_RANGE,
	LIST_TRIM,
	LIST_REMOVE,

	SET_ADD,
	SET_ALL,
	SET_REMOVE,
	SET_DISCARD,
	SET_LEN,
	SET_CONTAINS,
	SET_MOVE,
	SET_UNION,
	SET_UNION_INTO,
	SET_UNION_LEN,
	SET_INTERSECT,
	SET_INTERSECT_INTO,
	SET_INTERSECT_LEN,
	SET_DIFF,
	SET_DIFF_INTO,
	SET_DIFF_LEN,

	HASH_SET,
	HASH_ALL,
	HASH_GET,
	HASH_CONTAINS,
	HASH_LEN,
	HASH_KEY_LEN,
	HASH_REMOVE,
	HASH_KEYS,
	HASH_VALUES,
	HASH_UPDATE,
	HASH_MANY_GET,
};

// check if an operation is permitted for an unknown client
inline bool unknown_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::PING:
		case operation_t::EXIT:

		case operation_t::AUTH:
			return true;


		case operation_t::TERMINATE:
		case operation_t::PIPE:

		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::CHANGE_PASSWORD:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::RESET:
		case operation_t::KEYS:
		case operation_t::COPY_TO:
		case operation_t::MOVE_TO:
		case operation_t::COPY_TO_DB:
		case operation_t::MOVE_TO_DB:

		case operation_t::SET_EXPIRY:
		case operation_t::GET_EXPIRY:
		case operation_t::CLEAR_EXPIRY:

		case operation_t::GET:
		case operation_t::STR_SET:
		case operation_t::INT_SET:
		case operation_t::FLOAT_SET:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::STR_LEN:
		case operation_t::MANY_GET:

		case operation_t::LIST_LEN:
		case operation_t::LIST_GET:
		case operation_t::LIST_SET:
		case operation_t::LIST_PUSH_LEFT:
		case operation_t::LIST_PUSH_RIGHT:
		case operation_t::LIST_POP_LEFT:
		case operation_t::LIST_POP_RIGHT:
		case operation_t::LIST_RANGE:
		case operation_t::LIST_TRIM:
		case operation_t::LIST_REMOVE:

		case operation_t::SET_ADD:
		case operation_t::SET_ALL:
		case operation_t::SET_REMOVE:
		case operation_t::SET_DISCARD:
		case operation_t::SET_LEN:
		case operation_t::SET_CONTAINS:
		case operation_t::SET_MOVE:
		case operation_t::SET_UNION:
		case operation_t::SET_UNION_INTO:
		case operation_t::SET_UNION_LEN:
		case operation_t::SET_INTERSECT:
		case operation_t::SET_INTERSECT_INTO:
		case operation_t::SET_INTERSECT_LEN:
		case operation_t::SET_DIFF:
		case operation_t::SET_DIFF_INTO:
		case operation_t::SET_DIFF_LEN:

		case operation_t::HASH_SET:
		case operation_t::HASH_ALL:
		case operation_t::HASH_GET:
		case operation_t::HASH_CONTAINS:
		case operation_t::HASH_LEN:
		case operation_t::HASH_KEY_LEN:
		case operation_t::HASH_REMOVE:
		case operation_t::HASH_KEYS:
		case operation_t::HASH_VALUES:
		case operation_t::HASH_UPDATE:
		case operation_t::HASH_MANY_GET:

		default:
			return false;
	}
}

// check if an operation is permitted for a user
inline bool user_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::EXIT:
		case operation_t::PING:
		case operation_t::PIPE:

		case operation_t::CHANGE_PASSWORD:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::KEYS:
		case operation_t::COPY_TO:
		case operation_t::MOVE_TO:
		case operation_t::COPY_TO_DB:
		case operation_t::MOVE_TO_DB:

		case operation_t::SET_EXPIRY:
		case operation_t::GET_EXPIRY:
		case operation_t::CLEAR_EXPIRY:

		case operation_t::GET:
		case operation_t::STR_SET:
		case operation_t::INT_SET:
		case operation_t::FLOAT_SET:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::STR_LEN:
		case operation_t::MANY_GET:

		case operation_t::LIST_LEN:
		case operation_t::LIST_GET:
		case operation_t::LIST_SET:
		case operation_t::LIST_PUSH_LEFT:
		case operation_t::LIST_PUSH_RIGHT:
		case operation_t::LIST_POP_LEFT:
		case operation_t::LIST_POP_RIGHT:
		case operation_t::LIST_RANGE:
		case operation_t::LIST_TRIM:
		case operation_t::LIST_REMOVE:

		case operation_t::SET_ADD:
		case operation_t::SET_ALL:
		case operation_t::SET_REMOVE:
		case operation_t::SET_DISCARD:
		case operation_t::SET_LEN:
		case operation_t::SET_CONTAINS:
		case operation_t::SET_MOVE:
		case operation_t::SET_UNION:
		case operation_t::SET_UNION_INTO:
		case operation_t::SET_UNION_LEN:
		case operation_t::SET_INTERSECT:
		case operation_t::SET_INTERSECT_INTO:
		case operation_t::SET_INTERSECT_LEN:
		case operation_t::SET_DIFF:
		case operation_t::SET_DIFF_INTO:
		case operation_t::SET_DIFF_LEN:

		case operation_t::HASH_SET:
		case operation_t::HASH_ALL:
		case operation_t::HASH_GET:
		case operation_t::HASH_CONTAINS:
		case operation_t::HASH_LEN:
		case operation_t::HASH_KEY_LEN:
		case operation_t::HASH_REMOVE:
		case operation_t::HASH_KEYS:
		case operation_t::HASH_VALUES:
		case operation_t::HASH_UPDATE:
		case operation_t::HASH_MANY_GET:
			return true;


		case operation_t::TERMINATE:

		case operation_t::RESET:

		case operation_t::AUTH:
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
		case operation_t::EXIT:
		case operation_t::PING:
		case operation_t::TERMINATE:
		case operation_t::PIPE:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::RESET:
		case operation_t::KEYS:
		case operation_t::COPY_TO:
		case operation_t::MOVE_TO:
		case operation_t::COPY_TO_DB:
		case operation_t::MOVE_TO_DB:

		case operation_t::SET_EXPIRY:
		case operation_t::GET_EXPIRY:
		case operation_t::CLEAR_EXPIRY:

		case operation_t::GET:
		case operation_t::STR_SET:
		case operation_t::INT_SET:
		case operation_t::FLOAT_SET:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::STR_LEN:
		case operation_t::MANY_GET:

		case operation_t::LIST_LEN:
		case operation_t::LIST_GET:
		case operation_t::LIST_SET:
		case operation_t::LIST_PUSH_LEFT:
		case operation_t::LIST_PUSH_RIGHT:
		case operation_t::LIST_POP_LEFT:
		case operation_t::LIST_POP_RIGHT:
		case operation_t::LIST_RANGE:
		case operation_t::LIST_TRIM:
		case operation_t::LIST_REMOVE:

		case operation_t::SET_ADD:
		case operation_t::SET_ALL:
		case operation_t::SET_REMOVE:
		case operation_t::SET_DISCARD:
		case operation_t::SET_LEN:
		case operation_t::SET_CONTAINS:
		case operation_t::SET_MOVE:
		case operation_t::SET_UNION:
		case operation_t::SET_UNION_INTO:
		case operation_t::SET_UNION_LEN:
		case operation_t::SET_INTERSECT:
		case operation_t::SET_INTERSECT_INTO:
		case operation_t::SET_INTERSECT_LEN:
		case operation_t::SET_DIFF:
		case operation_t::SET_DIFF_INTO:
		case operation_t::SET_DIFF_LEN:

		case operation_t::HASH_SET:
		case operation_t::HASH_ALL:
		case operation_t::HASH_GET:
		case operation_t::HASH_CONTAINS:
		case operation_t::HASH_LEN:
		case operation_t::HASH_KEY_LEN:
		case operation_t::HASH_REMOVE:
		case operation_t::HASH_KEYS:
		case operation_t::HASH_VALUES:
		case operation_t::HASH_UPDATE:
		case operation_t::HASH_MANY_GET:
			return true;


		case operation_t::AUTH:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		default:
			return false;
	}
}

// check if an operation is permitted for an admin
inline bool admin_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::EXIT:
		case operation_t::PING:
		case operation_t::TERMINATE:
		case operation_t::PIPE:

		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::CHANGE_PASSWORD:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::RESET:
		case operation_t::EXISTS:
		case operation_t::KEYS:
		case operation_t::COPY_TO:
		case operation_t::MOVE_TO:
		case operation_t::COPY_TO_DB:
		case operation_t::MOVE_TO_DB:

		case operation_t::SET_EXPIRY:
		case operation_t::GET_EXPIRY:
		case operation_t::CLEAR_EXPIRY:

		case operation_t::GET:
		case operation_t::STR_SET:
		case operation_t::INT_SET:
		case operation_t::FLOAT_SET:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:
		case operation_t::STR_LEN:
		case operation_t::MANY_GET:

		case operation_t::LIST_LEN:
		case operation_t::LIST_GET:
		case operation_t::LIST_SET:
		case operation_t::LIST_PUSH_LEFT:
		case operation_t::LIST_PUSH_RIGHT:
		case operation_t::LIST_POP_LEFT:
		case operation_t::LIST_POP_RIGHT:
		case operation_t::LIST_RANGE:
		case operation_t::LIST_TRIM:
		case operation_t::LIST_REMOVE:

		case operation_t::SET_ADD:
		case operation_t::SET_ALL:
		case operation_t::SET_REMOVE:
		case operation_t::SET_DISCARD:
		case operation_t::SET_LEN:
		case operation_t::SET_CONTAINS:
		case operation_t::SET_MOVE:
		case operation_t::SET_UNION:
		case operation_t::SET_UNION_INTO:
		case operation_t::SET_UNION_LEN:
		case operation_t::SET_INTERSECT:
		case operation_t::SET_INTERSECT_INTO:
		case operation_t::SET_INTERSECT_LEN:
		case operation_t::SET_DIFF:
		case operation_t::SET_DIFF_INTO:
		case operation_t::SET_DIFF_LEN:

		case operation_t::HASH_SET:
		case operation_t::HASH_ALL:
		case operation_t::HASH_GET:
		case operation_t::HASH_CONTAINS:
		case operation_t::HASH_LEN:
		case operation_t::HASH_KEY_LEN:
		case operation_t::HASH_REMOVE:
		case operation_t::HASH_KEYS:
		case operation_t::HASH_VALUES:
		case operation_t::HASH_UPDATE:
		case operation_t::HASH_MANY_GET:
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
