//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_OPERATIONS_H
#define VANITY_OPERATIONS_H

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
	AUTH_LEVEL,
	USERNAME,

	SWITCH_DB,
	PERSIST,

	TRANSACT_BEGIN,
	TRANSACT_COMMIT,
	TRANSACT_DISCARD,

	PUBLISH,
	SUBSCRIBE,
	SUBSCRIBED,
	UNSUBSCRIBE,
	UNSUBSCRIBE_ALL,

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

	CLUSTER_JOIN,
	CLUSTER_KEY,
	CLUSTER_LEAVE,
	CLUSTER_NEW,

	PEERS,
};

const std::initializer_list<std::pair<operation_t, std::string>> OPERATION_T_STRINGS {
	{operation_t::EXIT,               "EXIT"},
	{operation_t::TERMINATE,          "TERMINATE"},
	{operation_t::PING,               "PING"},
	{operation_t::PIPE,               "PIPE"},

	{operation_t::AUTH_LEVEL,         "AUTH_LEVEL"},
	{operation_t::AUTH,               "AUTH"},
	{operation_t::ADD_USER,           "ADD_USER"},
	{operation_t::EDIT_USER,          "EDIT_USER"},
	{operation_t::DEL_USER,           "DEL_USER"},
	{operation_t::CHANGE_PASSWORD,    "CHANGE_PASSWORD"},
	{operation_t::USERNAME,           "USERNAME"},

	{operation_t::SWITCH_DB,          "SWITCH_DB"},
	{operation_t::PERSIST,            "PERSIST"},

	{operation_t::TRANSACT_BEGIN,     "TRANSACT_BEGIN"},
	{operation_t::TRANSACT_COMMIT,    "TRANSACT_COMMIT"},
	{operation_t::TRANSACT_DISCARD,   "TRANSACT_DISCARD"},

	{operation_t::PUBLISH,            "PUBLISH"},
	{operation_t::SUBSCRIBED,         "SUBSCRIBED"},
	{operation_t::SUBSCRIBE,          "SUBSCRIBE"},
	{operation_t::UNSUBSCRIBE_ALL,    "UNSUBSCRIBE_ALL"},
	{operation_t::UNSUBSCRIBE,        "UNSUBSCRIBE"},

	{operation_t::DEL,                "DEL"},
	{operation_t::TYPE,               "TYPE"},
	{operation_t::EXISTS,             "EXISTS"},
	{operation_t::RESET,              "RESET"},
	{operation_t::KEYS,               "KEYS"},
	{operation_t::COPY_TO_DB,         "COPY_TO_DB"},
	{operation_t::MOVE_TO_DB,         "MOVE_TO_DB"},
	{operation_t::COPY_TO,            "COPY_TO"},
	{operation_t::MOVE_TO,            "MOVE_TO"},

	{operation_t::SET_EXPIRY,         "SET_EXPIRY"},
	{operation_t::GET_EXPIRY,         "GET_EXPIRY"},
	{operation_t::CLEAR_EXPIRY,       "CLEAR_EXPIRY"},

	{operation_t::GET,                "GET"},
	{operation_t::STR_SET,            "STR_SET"},
	{operation_t::INT_SET,            "INT_SET"},
	{operation_t::FLOAT_SET,          "FLOAT_SET"},
	{operation_t::INCR_INT,           "INCR_INT"},
	{operation_t::INCR_FLOAT,         "INCR_FLOAT"},
	{operation_t::STR_LEN,            "STR_LEN"},
	{operation_t::MANY_GET,           "MANY_GET"},

	{operation_t::LIST_LEN,           "LIST_LEN"},
	{operation_t::LIST_GET,           "LIST_GET"},
	{operation_t::LIST_SET,           "LIST_SET"},
	{operation_t::LIST_PUSH_LEFT,     "LIST_PUSH_LEFT"},
	{operation_t::LIST_PUSH_RIGHT,    "LIST_PUSH_RIGHT"},
	{operation_t::LIST_POP_LEFT,      "LIST_POP_LEFT"},
	{operation_t::LIST_POP_RIGHT,     "LIST_POP_RIGHT"},
	{operation_t::LIST_RANGE,         "LIST_RANGE"},
	{operation_t::LIST_TRIM,          "LIST_TRIM"},
	{operation_t::LIST_REMOVE,        "LIST_REMOVE"},

	{operation_t::SET_ADD,            "SET_ADD"},
	{operation_t::SET_ALL,            "SET_ALL"},
	{operation_t::SET_REMOVE,         "SET_REMOVE"},
	{operation_t::SET_DISCARD,        "SET_DISCARD"},
	{operation_t::SET_LEN,            "SET_LEN"},
	{operation_t::SET_CONTAINS,       "SET_CONTAINS"},
	{operation_t::SET_MOVE,           "SET_MOVE"},
	{operation_t::SET_UNION_INTO,     "SET_UNION_INTO"},
	{operation_t::SET_UNION_LEN,      "SET_UNION_LEN"},
	{operation_t::SET_UNION,          "SET_UNION"},
	{operation_t::SET_INTERSECT_INTO, "SET_INTERSECT_INTO"},
	{operation_t::SET_INTERSECT_LEN,  "SET_INTERSECT_LEN"},
	{operation_t::SET_INTERSECT,      "SET_INTERSECT"},
	{operation_t::SET_DIFF_INTO,      "SET_DIFF_INTO"},
	{operation_t::SET_DIFF_LEN,       "SET_DIFF_LEN"},
	{operation_t::SET_DIFF,           "SET_DIFF"},

	{operation_t::HASH_SET,           "HASH_SET"},
	{operation_t::HASH_ALL,           "HASH_ALL"},
	{operation_t::HASH_GET,           "HASH_GET"},
	{operation_t::HASH_CONTAINS,      "HASH_CONTAINS"},
	{operation_t::HASH_LEN,           "HASH_LEN"},
	{operation_t::HASH_KEY_LEN,       "HASH_KEY_LEN"},
	{operation_t::HASH_REMOVE,        "HASH_REMOVE"},
	{operation_t::HASH_KEYS,          "HASH_KEYS"},
	{operation_t::HASH_VALUES,        "HASH_VALUES"},
	{operation_t::HASH_UPDATE,        "HASH_UPDATE"},
	{operation_t::HASH_MANY_GET,      "HASH_MANY_GET"},

	{operation_t::CLUSTER_JOIN,       "CLUSTER_JOIN"},
	{operation_t::CLUSTER_KEY,        "CLUSTER_KEY"},
	{operation_t::CLUSTER_LEAVE,      "CLUSTER_LEAVE"},
	{operation_t::CLUSTER_NEW,        "CLUSTER_NEW"},

	{operation_t::PEERS,              "PEERS"},
};

// check if an operation is permitted for an unknown client
inline bool unknown_is_permitted(operation_t operation)
{
	switch (operation)
	{
		case operation_t::PING:
		case operation_t::EXIT:

		case operation_t::AUTH:
		case operation_t::AUTH_LEVEL:
			return true;


		case operation_t::TERMINATE:
		case operation_t::PIPE:

		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::USERNAME:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::PUBLISH:
		case operation_t::SUBSCRIBE:
		case operation_t::SUBSCRIBED:
		case operation_t::UNSUBSCRIBE:
		case operation_t::UNSUBSCRIBE_ALL:

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

		case operation_t::CLUSTER_JOIN:
		case operation_t::CLUSTER_KEY:
		case operation_t::CLUSTER_LEAVE:
		case operation_t::CLUSTER_NEW:

		case operation_t::PEERS:
			return false;

		default:
			throw std::runtime_error("invalid operation");
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
		case operation_t::AUTH_LEVEL:
		case operation_t::USERNAME:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::PUBLISH:
		case operation_t::SUBSCRIBE:
		case operation_t::SUBSCRIBED:
		case operation_t::UNSUBSCRIBE:
		case operation_t::UNSUBSCRIBE_ALL:

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

		case operation_t::CLUSTER_JOIN:
		case operation_t::CLUSTER_KEY:
		case operation_t::CLUSTER_LEAVE:
		case operation_t::CLUSTER_NEW:

		case operation_t::PEERS:
			return false;

		default:
			throw std::runtime_error("invalid operation");
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

		case operation_t::PEERS:
			return true;


		case operation_t::AUTH:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::AUTH_LEVEL:
		case operation_t::USERNAME:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::PUBLISH:
		case operation_t::SUBSCRIBE:
		case operation_t::SUBSCRIBED:
		case operation_t::UNSUBSCRIBE:
		case operation_t::UNSUBSCRIBE_ALL:

		case operation_t::CLUSTER_JOIN:
		case operation_t::CLUSTER_KEY:
		case operation_t::CLUSTER_LEAVE:
		case operation_t::CLUSTER_NEW:
			return false;

		default:
			throw std::runtime_error("invalid operation");
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
		case operation_t::AUTH_LEVEL:
		case operation_t::USERNAME:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::PUBLISH:
		case operation_t::SUBSCRIBE:
		case operation_t::SUBSCRIBED:
		case operation_t::UNSUBSCRIBE:
		case operation_t::UNSUBSCRIBE_ALL:

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

		case operation_t::CLUSTER_JOIN:
		case operation_t::CLUSTER_KEY:
		case operation_t::CLUSTER_LEAVE:
		case operation_t::CLUSTER_NEW:

		case operation_t::PEERS:
			return true;


		case operation_t::AUTH:
			return false;

		default:
			throw std::runtime_error("invalid operation");
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
			throw std::runtime_error("invalid client_auth");
	}
}

// possible behaviours for an operation in some context
enum class behaviour_t {
	DEFAULT,
	CONTEXTUAL,
	NOT_PERMITTED,
};

// get the behaviour of an operation during a normal connection
inline behaviour_t normal_behaviour(operation_t operation)
{
	switch (operation)
	{
		case operation_t::PING:
		case operation_t::EXIT:
		case operation_t::TERMINATE:
		case operation_t::PIPE:

		case operation_t::AUTH:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::AUTH_LEVEL:
		case operation_t::USERNAME:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:

		case operation_t::PUBLISH:
		case operation_t::SUBSCRIBE:
		case operation_t::SUBSCRIBED:
		case operation_t::UNSUBSCRIBE:
		case operation_t::UNSUBSCRIBE_ALL:

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

		case operation_t::CLUSTER_JOIN:
		case operation_t::CLUSTER_KEY:
		case operation_t::CLUSTER_LEAVE:
		case operation_t::CLUSTER_NEW:

		case operation_t::PEERS:
			return behaviour_t::DEFAULT;


		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:
			return behaviour_t::NOT_PERMITTED;

		default:
			throw std::runtime_error("invalid operation");
	}
}

// get the behaviour of an operation during a transaction
inline behaviour_t transaction_behaviour(operation_t operation)
{
	switch (operation)
	{
		case operation_t::PING:
		case operation_t::EXIT:
		case operation_t::TERMINATE:

		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::PUBLISH:
		case operation_t::SUBSCRIBE:
		case operation_t::SUBSCRIBED:
		case operation_t::UNSUBSCRIBE:
		case operation_t::UNSUBSCRIBE_ALL:

		case operation_t::CLUSTER_JOIN:
		case operation_t::CLUSTER_KEY:
		case operation_t::CLUSTER_LEAVE:
		case operation_t::CLUSTER_NEW:

		case operation_t::PEERS:
			return behaviour_t::DEFAULT;


		case operation_t::PIPE:

		case operation_t::AUTH:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::AUTH_LEVEL:
		case operation_t::USERNAME:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::COPY_TO_DB:
		case operation_t::MOVE_TO_DB:

		case operation_t::TRANSACT_BEGIN:

		case operation_t::SET_EXPIRY:
			return behaviour_t::NOT_PERMITTED;


		case operation_t::DEL:
		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::RESET:
		case operation_t::KEYS:
		case operation_t::COPY_TO:
		case operation_t::MOVE_TO:

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
			return behaviour_t::CONTEXTUAL;

		default:
			throw std::runtime_error("invalid operation");
	}
}

// get the behaviour of an operation in some context
inline behaviour_t behaviour(operation_t operation, conn_state state)
{
	switch (state)
	{
		case conn_state::NORMAL:
			return normal_behaviour(operation);
		case conn_state::TRANSACTION:
			return transaction_behaviour(operation);
		default:
			throw std::runtime_error("invalid conn_state");
	}
}

// check if an operation should be WALed
inline bool should_wal(operation_t operation)
{
	switch (operation)
	{
		case operation_t::EXIT:
		case operation_t::PING:
		case operation_t::TERMINATE:
		case operation_t::PIPE:

		case operation_t::AUTH:
		case operation_t::ADD_USER:
		case operation_t::EDIT_USER:
		case operation_t::DEL_USER:
		case operation_t::CHANGE_PASSWORD:
		case operation_t::AUTH_LEVEL:
		case operation_t::USERNAME:

		case operation_t::SWITCH_DB:
		case operation_t::PERSIST:

		case operation_t::TRANSACT_BEGIN:
		case operation_t::TRANSACT_COMMIT:
		case operation_t::TRANSACT_DISCARD:

		case operation_t::PUBLISH:
		case operation_t::SUBSCRIBE:
		case operation_t::SUBSCRIBED:
		case operation_t::UNSUBSCRIBE:
		case operation_t::UNSUBSCRIBE_ALL:


		case operation_t::TYPE:
		case operation_t::EXISTS:
		case operation_t::KEYS:

		case operation_t::GET_EXPIRY:
		case operation_t::SET_EXPIRY:

		case operation_t::GET:
		case operation_t::STR_LEN:
		case operation_t::MANY_GET:

		case operation_t::LIST_LEN:
		case operation_t::LIST_GET:
		case operation_t::LIST_RANGE:

		case operation_t::SET_ALL:
		case operation_t::SET_LEN:
		case operation_t::SET_CONTAINS:
		case operation_t::SET_UNION:
		case operation_t::SET_UNION_LEN:
		case operation_t::SET_INTERSECT:
		case operation_t::SET_INTERSECT_LEN:
		case operation_t::SET_DIFF:
		case operation_t::SET_DIFF_LEN:

		case operation_t::HASH_ALL:
		case operation_t::HASH_GET:
		case operation_t::HASH_CONTAINS:
		case operation_t::HASH_LEN:
		case operation_t::HASH_KEY_LEN:
		case operation_t::HASH_KEYS:
		case operation_t::HASH_VALUES:
		case operation_t::HASH_MANY_GET:

		case operation_t::CLUSTER_JOIN:
		case operation_t::CLUSTER_KEY:
		case operation_t::CLUSTER_LEAVE:
		case operation_t::CLUSTER_NEW:

		case operation_t::PEERS:
			return false;


		case operation_t::DEL:
		case operation_t::RESET:
		case operation_t::COPY_TO:
		case operation_t::MOVE_TO:
		case operation_t::COPY_TO_DB:
		case operation_t::MOVE_TO_DB:

		case operation_t::CLEAR_EXPIRY:

		case operation_t::STR_SET:
		case operation_t::INT_SET:
		case operation_t::FLOAT_SET:
		case operation_t::INCR_INT:
		case operation_t::INCR_FLOAT:

		case operation_t::LIST_SET:
		case operation_t::LIST_PUSH_LEFT:
		case operation_t::LIST_PUSH_RIGHT:
		case operation_t::LIST_POP_LEFT:
		case operation_t::LIST_POP_RIGHT:
		case operation_t::LIST_TRIM:
		case operation_t::LIST_REMOVE:

		case operation_t::SET_ADD:
		case operation_t::SET_REMOVE:
		case operation_t::SET_DISCARD:
		case operation_t::SET_MOVE:
		case operation_t::SET_UNION_INTO:
		case operation_t::SET_INTERSECT_INTO:
		case operation_t::SET_DIFF_INTO:

		case operation_t::HASH_SET:
		case operation_t::HASH_REMOVE:
		case operation_t::HASH_UPDATE:
			return true;

		default:
			throw std::runtime_error("invalid operation");
	}
}

} // namespace vanity

#endif //VANITY_OPERATIONS_H
