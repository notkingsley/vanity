//
// Created by kingsli on 9/14/23.
//

#include <stdexcept>
#include <tuple>

#include "request_server.h"


namespace vanity {

// all object types
enum class object_t{
	STR,
	INT,
	FLOAT,
	ARR,
};

template<object_t _obj>
struct concrete_traits;

template<>
struct concrete_traits<object_t::STR> {
	using type = std::string;
};

template<>
struct concrete_traits<object_t::INT> {
	using type = int64_t;
};

template<>
struct concrete_traits<object_t::FLOAT> {
	using type = double;
};

template<>
struct concrete_traits<object_t::ARR> {
	using type = std::vector<std::string>;
};

template <object_t ...Args> struct concrete {
	using type = std::tuple<typename concrete_traits<Args>::type...>;
};

template<object_t _obj>
struct concrete<_obj> {
	using type = typename concrete_traits<_obj>::type;
};

template<object_t ...Args>
using concrete_t = typename concrete<Args...>::type;

// increment pos until it is not a whitespace
static inline void skip_whitespace(const std::string& msg, size_t& pos)
{
	while (pos < msg.size() and isspace(msg[pos]))
		++pos;
}

// ensure we are at the end of the message
static inline void ensure_end(const std::string& msg, size_t& pos)
{
	skip_whitespace(msg, pos);
	if (pos < msg.size())
		throw InvalidRequest("unexpected character at end of message");
}

// ensure we are not at the end of the message
static inline void ensure_not_end(const std::string& msg, size_t& pos)
{
	skip_whitespace(msg, pos);
	if (pos >= msg.size())
		throw InvalidRequest("unexpected end of message");
}

// extract the operation from the message
static inline operation_t extract_operation(const std::string& msg, size_t& pos)
{
	static const std::initializer_list <std::pair<operation_t, std::string>> operations {
		{operation_t::ADD_USER,  "ADD_USER"},
		{operation_t::EDIT_USER, "EDIT_USER"},
		{operation_t::DEL_USER,  "DEL_USER"},
		{operation_t::GET,       "GET"},
		{operation_t::SET,       "SET"},
		{operation_t::DEL,       "DEL"},
		{operation_t::TYPE,      "TYPE"},
		{operation_t::EXISTS,    "EXISTS"},
		{operation_t::INCR_INT,  "INCR_INT"},
		{operation_t::INCR_FLOAT,"INCR_FLOAT"},
		{operation_t::LEN_STR,   "LEN_STR"},
		{operation_t::MANY_GET,  "MANY_GET"},
		{operation_t::SWITCH_DB, "SWITCH_DB"},
		{operation_t::AUTH,      "AUTH"},
		{operation_t::CHANGE_PASSWORD, "CHANGE_PASSWORD"},
		{operation_t::PERSIST,   "PERSIST"},
		{operation_t::EXIT,      "EXIT"},
		{operation_t::TERMINATE, "TERMINATE"},
		{operation_t::RESET,     "RESET"},
		{operation_t::PING,      "PING"},
	};
	skip_whitespace(msg, pos);
	for (const auto& [op, str] : operations) {
		if (msg.compare(pos, str.size(), str) == 0) {
			pos += str.size();
			return op;
		}
	}
	throw InvalidRequest("invalid operation");
}

// extract the object type from the message
static inline object_t extract_object_t(const std::string& msg, size_t& pos)
{
	static const char object_separator = ':';
	static const std::initializer_list <std::pair<object_t, std::string>> objects {
		{object_t::STR,   "STR"},
		{object_t::INT,   "INT"},
		{object_t::FLOAT, "FLOAT"},
		{object_t::ARR,   "ARR"},
	};

	skip_whitespace(msg, pos);
	if (msg[pos] != object_separator) {
		throw InvalidRequest("expected object type");
	}
	++pos;

	skip_whitespace(msg, pos);
	for (const auto& [obj, str] : objects) {
		if (msg.compare(pos, str.size(), str) == 0) {
			pos += str.size();
			return obj;
		}
	}
	throw InvalidRequest("invalid object type");
}

// extract thea client auth level from the message
static inline client_auth extract_client_auth(const std::string& msg, size_t& pos)
{
	static const std::initializer_list <std::pair<client_auth, std::string>> auths {
		{client_auth::UNKNOWN,     "UNKNOWN"},
		{client_auth::USER,        "USER"},
		{client_auth::ADMIN,       "ADMIN"},
		{client_auth::PEER,        "PEER"},
	};

	skip_whitespace(msg, pos);
	for (const auto& [auth, str] : auths) {
		if (msg.compare(pos, str.size(), str) == 0) {
			pos += str.size();
			return auth;
		}
	}
	throw InvalidRequest("invalid auth level");
}

// extract a (len) from part of a message
static inline size_t extract_len(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
	if (msg[pos] != '(') {
		throw InvalidRequest("expected length specifier");
	}
	++pos;

	try{
		size_t count = 0;
		size_t len = std::stoull(msg.substr(pos), &count);
		pos += count;
		if (msg[pos] != ')') {
			throw InvalidRequest("expected length specifier");
		}
		++pos;
		return len;
	}
	catch (const std::out_of_range& e) {
		throw InvalidRequest("length out of range");
	}
	catch (const std::invalid_argument& e) {
		throw InvalidRequest("invalid length");
	}
}

// extract an object from part of a message
template<object_t ...Args>
static inline concrete_t<Args...> extract(const std::string& msg, size_t& pos)
{
	return {extract<Args>(msg, pos)...};
}

// extract exactly the given object types from the rest of a message
// throw if there are more or less than the given object types
template<object_t ...Args>
static inline concrete_t<Args...> extract_exact(const std::string& msg, size_t& pos)
{
	auto ret = extract<Args...>(msg, pos);
	ensure_end(msg, pos);
	return ret;
}

// extract an int64_t from part of a message
template<>
inline int64_t extract<object_t::INT>(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
	try{
		size_t count = 0;
		auto ret{std::stoll(msg.substr(pos), &count)};
		pos += count;
		return ret;
	}
	catch (const std::out_of_range& e) {
		throw InvalidRequest("integer out of range");
	}
	catch (const std::invalid_argument& e) {
		throw InvalidRequest("invalid integer");
	}
}

// extract a double from part of a message
template<>
inline double extract<object_t::FLOAT>(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
	try{
		size_t count = 0;
		auto ret{std::stod(msg.substr(pos), &count)};
		pos += count;
		return ret;
	}
	catch (const std::out_of_range& e) {
		throw InvalidRequest("float out of range");
	}
	catch (const std::invalid_argument& e) {
		throw InvalidRequest("invalid float");
	}
}

// extract a string from part of a message
template<>
inline std::string extract<object_t::STR>(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
	if (msg[pos] != '"') {
		throw InvalidRequest("word not opened with quotes");
	}
	++pos;

	std::string word;
	while (pos < msg.size()) {
		if (msg[pos] == '"') {
			++pos;
			return word;
		}
		if (msg[pos] == '\\' and pos + 1 < msg.size() and msg[pos + 1] == '"') {
			++pos;
		}
		word += msg[pos];
		++pos;
	}

	throw InvalidRequest("word not closed with quotes");
}

// extract a vector of strings from part of a message
template<>
inline std::vector<std::string> extract<object_t::ARR>(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
	size_t len = extract_len(msg, pos);
	if (msg[pos] != '[')
		throw InvalidRequest("array not opened with bracket");
	++pos;

	std::vector<std::string> arr;
	arr.reserve(len);
	for (size_t i = 0; i < len; ++i)
		arr.emplace_back(extract<object_t::STR>(msg, pos));

	if (msg[pos] != ']')
		throw InvalidRequest("array not closed with bracket");
	++pos;

	return arr;
}


void RequestServer::handle(const std::string& msg, Client& client) {
	try{
		size_t pos = 0;
		operation_t op = extract_operation(msg, pos);

		if (not client.has_perm(op))
			return send(client, denied());

		using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR;
		switch (op) {
			case operation_t::GET:
			{
				request_get(client, extract_exact<STR>(msg, pos));
				break;
			}
			case operation_t::SET:
			{
				dispatch_set(client, msg, pos);
				break;
			}
			case operation_t::DEL:
			{
				request_del(client, extract_exact<STR>(msg, pos));
				break;
			}
			case operation_t::TYPE:
			{
				request_type(client, extract_exact<STR>(msg, pos));
				break;
			}
			case operation_t::EXISTS:
			{
				request_exists(client, extract_exact<STR>(msg, pos));
				break;
			}
			case operation_t::INCR_INT:
			{
				auto [key, value] = extract_exact<STR, INT>(msg, pos);
				request_incr_int(client, key, value);
				break;
			}
			case operation_t::INCR_FLOAT:
			{
				auto [key, value] = extract_exact<STR, FLOAT>(msg, pos);
				request_incr_float(client, key, value);
				break;
			}
			case operation_t::LEN_STR:
			{
				request_len_str(client, extract_exact<STR>(msg, pos));
				break;
			}
			case operation_t::MANY_GET:
			{
				request_many_get(client, extract_exact<ARR>(msg, pos));
				break;
			}
			case operation_t::SWITCH_DB:
			{
				request_switch_db(client, extract_exact<INT>(msg, pos));
				break;
			}
			case operation_t::ADD_USER:
			{
				auto [username, password] = extract_exact<STR, STR>(msg, pos);
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
				request_del_user(client, extract_exact<STR>(msg, pos));
				break;
			}
			case operation_t::AUTH:
			{
				auto [username, password] = extract_exact<STR, STR>(msg, pos);
				request_auth(client, username, password);
				break;
			}
			case operation_t::CHANGE_PASSWORD:
			{
				request_change_password(client, extract_exact<STR>(msg, pos));
				break;
			}
			case operation_t::PERSIST:
			{
				ensure_end(msg, pos);
				request_persist(client);
				break;
			}
			case operation_t::EXIT:
			{
				ensure_end(msg, pos);
				request_exit(client);
				break;
			}
			case operation_t::TERMINATE:
			{
				ensure_end(msg, pos);
				request_terminate(client);
				break;
			}
			case operation_t::RESET:
			{
				ensure_end(msg, pos);
				request_reset(client);
				break;
			}
			case operation_t::PING:
			{
				request_ping(client, msg.substr(pos));
				break;
			}
		}
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

void RequestServer::dispatch_set(Client &client, const std::string &msg, size_t &pos) {
	using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR;

	object_t obj = extract_object_t(msg, pos);
	std::string key = extract<STR>(msg, pos);

	switch (obj) {
		case STR:
		{
			auto value {extract_exact<STR>(msg, pos)};
			request_set(client, key, value);
			break;
		}
		case INT:
		{
			auto value {extract_exact<INT>(msg, pos)};
			request_set(client, key, value);
			break;
		}
		case FLOAT:
		{
			auto value {extract_exact<FLOAT>(msg, pos)};
			request_set(client, key, value);
			break;
		}
		case ARR:
		{
			throw InvalidRequest("invalid object type: use MANY_SET or LIST_SET");
		}
	}
}

} // namespace vanity
