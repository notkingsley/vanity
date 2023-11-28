//
// Created by kingsli on 9/14/23.
//

#include "extract.h"
#include "request_server.h"

namespace vanity {

void RequestServer::handle(const std::string& msg, Client& client) {
	size_t pos = 0;
	do_handle(client, msg, pos, true);
}

void RequestServer::do_handle(Client &client, const std::string &msg, size_t &pos, bool end) {
	try{
		operation_t op = extract_operation(msg, pos);
		if (not client.has_perm(op))
			return send(client, denied());

		return dispatch_op(client, op, msg, pos, end);
	}
	catch (const InvalidRequest& e)
	{
		send(client, bad_request(e.what()));
	}
	catch (const DestroyClient& e)
	{
		logger().debug("Client disconnecting");
		throw;
	}
	catch (const Exception& e)
	{
		logger().error(e.what());
		send(client, internal_error(e.what()));
	}
	catch (const std::exception& e)
	{
		logger().error(e.what());
		send(client, internal_error(e.what()));
	}
	catch (...)
	{
		logger().error("unknown error with message: " + msg);
		send(client, internal_error("unknown error"));
	}
}

void RequestServer::dispatch_set(Client &client, const std::string &msg, size_t &pos, bool end) {
	using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR, object_t::LIST, object_t::SET, object_t::HASH;

	object_t obj = extract_object_t(msg, pos);
	std::string key = extract<STR>(msg, pos);

	switch (obj) {
		case STR:
		{
			auto value {extract_exact<STR>(msg, pos, end)};
			request_set_str(client, key, value);
			break;
		}
		case INT:
		{
			auto value {extract_exact<INT>(msg, pos, end)};
			request_set_int(client, key, value);
			break;
		}
		case FLOAT:
		{
			auto value {extract_exact<FLOAT>(msg, pos, end)};
			request_set_float(client, key, value);
			break;
		}
		case ARR:	// fallthrough
		case LIST:	// fallthrough
		case SET:	// fallthrough
		case HASH:	// fallthrough
			throw InvalidRequest("invalid object type:");
	}
}

void RequestServer::dispatch_op(Client &client, operation_t op, const std::string &msg, size_t &pos, bool expect_end) {
	using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR, object_t::LIST, object_t::SET, object_t::HASH;
	switch (op) {
		case operation_t::TERMINATE:
		{
			ensure_end(msg, pos);
			request_terminate(client);
			break;
		}
		case operation_t::PING:
		{
			request_ping(client, msg.substr(pos));
			break;
		}
		case operation_t::EXIT:
		{
			ensure_end(msg, pos);
			request_exit(client);
			break;
		}
		case operation_t::PIPE:
		{
			request_pipe(client, msg, pos);
			break;
		}

		case operation_t::AUTH:
		{
			auto [username, password] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_auth(client, username, password);
			break;
		}
		case operation_t::ADD_USER:
		{
			auto [username, password] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_add_user(client, username, password);
			break;
		}
		case operation_t::EDIT_USER:
		{
			auto username = extract<STR>(msg, pos);
			auto auth = extract_client_auth(msg, pos);
			ensure_end(msg, pos);
			request_edit_user(client, username, auth);
			break;
		}
		case operation_t::DEL_USER:
		{
			request_del_user(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::CHANGE_PASSWORD:
		{
			request_change_password(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}

		case operation_t::SWITCH_DB:
		{
			request_switch_db(client, extract_exact<INT>(msg, pos, expect_end));
			break;
		}
		case operation_t::PERSIST:
		{
			ensure_end(msg, pos);
			request_persist(client);
			break;
		}

		case operation_t::DEL:
		{
			request_del(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::TYPE:
		{
			request_type(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::EXISTS:
		{
			request_exists(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::RESET:
		{
			ensure_end(msg, pos);
			request_reset(client);
			break;
		}

		case operation_t::SET_EXPIRY:
		{
			auto [key, expiry] = extract_exact<STR, FLOAT>(msg, pos, expect_end);
			request_set_expiry(client, key, expiry);
			break;
		}
		case operation_t::GET_EXPIRY:
		{
			request_get_expiry(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::CLEAR_EXPIRY:
		{
			request_clear_expiry(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}

		case operation_t::GET:
		{
			request_get(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::SET:
		{
			dispatch_set(client, msg, pos, expect_end);
			break;
		}
		case operation_t::INCR_INT:
		{
			auto [key, value] = extract_exact<STR, INT>(msg, pos, expect_end);
			request_incr_int(client, key, value);
			break;
		}
		case operation_t::INCR_FLOAT:
		{
			auto [key, value] = extract_exact<STR, FLOAT>(msg, pos, expect_end);
			request_incr_float(client, key, value);
			break;
		}
		case operation_t::STR_LEN:
		{
			request_str_len(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::MANY_GET:
		{
			request_many_get(client, extract_exact<ARR>(msg, pos, expect_end));
			break;
		}

		case operation_t::LIST_LEN:
		{
			request_list_len(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::LIST_GET:
		{
			auto [key, index] = extract_exact<STR, INT>(msg, pos, expect_end);
			request_list_get(client, key, index);
			break;
		}
		case operation_t::LIST_SET:
		{
			auto [key, index, value] = extract_exact<STR, INT, STR>(msg, pos, expect_end);
			request_list_set(client, key, index, value);
			break;
		}
		case operation_t::LIST_PUSH_LEFT:
		{
			auto [key, value] = extract_exact<STR, LIST>(msg, pos, expect_end);
			request_list_push_left(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_PUSH_RIGHT:
		{
			auto [key, value] = extract_exact<STR, LIST>(msg, pos, expect_end);
			request_list_push_right(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_POP_LEFT:
		{
			auto [key, count] = extract_exact<STR, INT>(msg, pos, expect_end);
			request_list_pop_left(client, key, count);
			break;
		}
		case operation_t::LIST_POP_RIGHT:
		{
			auto [key, count] = extract_exact<STR, INT>(msg, pos, expect_end);
			request_list_pop_right(client, key, count);
			break;
		}
		case operation_t::LIST_RANGE:
		{
			auto [key, start, stop] = extract_exact<STR, INT, INT>(msg, pos, expect_end);
			request_list_range(client, key, start, stop);
			break;
		}
		case operation_t::LIST_TRIM:
		{
			auto [key, start, stop] = extract_exact<STR, INT, INT>(msg, pos, expect_end);
			request_list_trim(client, key, start, stop);
			break;
		}
		case operation_t::LIST_REMOVE:
		{
			auto [key, value, count] = extract_exact<STR, STR, INT>(msg, pos, expect_end);
			request_list_remove(client, key, value, count);
			break;
		}

		case operation_t::SET_ADD:
		{
			auto [key, values] = extract_exact<STR, SET>(msg, pos, expect_end);
			request_set_add(client, key, std::move(values));
			break;
		}
		case operation_t::SET_ALL:
		{
			request_set_all(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::SET_REMOVE:
		{
			auto [key, count] = extract_exact<STR, INT>(msg, pos, expect_end);
			request_set_remove(client, key, count);
			break;
		}
		case operation_t::SET_DISCARD:
		{
			auto [key, values] = extract_exact<STR, SET>(msg, pos, expect_end);
			request_set_discard(client, key, std::move(values));
			break;
		}
		case operation_t::SET_LEN:
		{
			request_set_len(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::SET_CONTAINS:
		{
			auto [key, value] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_set_contains(client, key, value);
			break;
		}
		case operation_t::SET_MOVE:
		{
			auto [source, dest, value] = extract_exact<STR, STR, STR>(msg, pos, expect_end);
			request_set_move(client, source, dest, value);
			break;
		}
		case operation_t::SET_UNION:
		{
			request_set_union(client, extract_exact<ARR>(msg, pos, expect_end));
			break;
		}
		case operation_t::SET_UNION_INTO:
		{
			auto [dest, keys] = extract_exact<STR, ARR>(msg, pos, expect_end);
			request_set_union_into(client, dest, keys);
			break;
		}
		case operation_t::SET_UNION_LEN:
		{
			request_set_union_len(client, extract_exact<ARR>(msg, pos, expect_end));
			break;
		}
		case operation_t::SET_INTERSECT:
		{
			request_set_intersection(client, extract_exact<ARR>(msg, pos, expect_end));
			break;
		}
		case operation_t::SET_INTERSECT_INTO:
		{
			auto [dest, keys] = extract_exact<STR, ARR>(msg, pos, expect_end);
			request_set_intersection_into(client, dest, keys);
			break;
		}
		case operation_t::SET_INTERSECT_LEN:
		{
			request_set_intersection_len(client, extract_exact<ARR>(msg, pos, expect_end));
			break;
		}
		case operation_t::SET_DIFF:
		{
			auto [key1, key2] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_set_difference(client, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_INTO:
		{
			auto [dest, key1, key2] = extract_exact<STR, STR, STR>(msg, pos, expect_end);
			request_set_difference_into(client, dest, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_LEN:
		{
			auto [key1, key2] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_set_difference_len(client, key1, key2);
			break;
		}

		case operation_t::HASH_SET:
		{
			auto [key, hash] = extract_exact<STR, HASH>(msg, pos, expect_end);
			request_hash_set(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_ALL:
		{
			request_hash_all(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::HASH_GET:
		{
			auto [key, hash_key] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_hash_get(client, key, hash_key);
			break;
		}
		case operation_t::HASH_CONTAINS:
		{
			auto [key, hash_key] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_hash_contains(client, key, hash_key);
			break;
		}
		case operation_t::HASH_LEN:
		{
			request_hash_len(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::HASH_KEY_LEN:
		{
			auto [key, hash_key] = extract_exact<STR, STR>(msg, pos, expect_end);
			request_hash_key_len(client, key, hash_key);
			break;
		}
		case operation_t::HASH_REMOVE:
		{
			auto [key, hash_key] = extract_exact<STR, ARR>(msg, pos, expect_end);
			request_hash_remove(client, key, hash_key);
			break;
		}
		case operation_t::HASH_KEYS:
		{
			request_hash_keys(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::HASH_VALUES:
		{
			request_hash_values(client, extract_exact<STR>(msg, pos, expect_end));
			break;
		}
		case operation_t::HASH_UPDATE:
		{
			auto [key, hash] = extract_exact<STR, HASH>(msg, pos, expect_end);
			request_hash_update(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_MANY_GET:
		{
			auto [key, hash_keys] = extract_exact<STR, ARR>(msg, pos, expect_end);
			request_hash_many_get(client, key, hash_keys);
			break;
		}
	}
}

} // namespace vanity
