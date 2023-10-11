//
// Created by kingsli on 9/14/23.
//

#include <stdexcept>
#include "request_server.h"


namespace vanity {

// all object types
enum class object_t{
	STR,
	INT,
	FLOAT,
};

// the concrete type of object_t
template <object_t>
struct concrete;

template <>
struct concrete<object_t::STR> {
	using type = std::string;
};

template <>
struct concrete<object_t::INT> {
	using type = int64_t;
};

template <>
struct concrete<object_t::FLOAT> {
	using type = double;
};

template<object_t _obj>
using concrete_t = typename concrete<_obj>::type;

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
	static std::initializer_list <std::pair<operation_t, std::string>> operations {
		{operation_t::ADD_USER,  "ADD_USER"},
		{operation_t::EDIT_USER, "EDIT_USER"},
		{operation_t::DEL_USER,  "DEL_USER"},
		{operation_t::GET,       "GET"},
		{operation_t::SET,       "SET"},
		{operation_t::DEL,       "DEL"},
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
	static char object_separator = ':';
	static std::initializer_list <std::pair<object_t, std::string>> objects {
		{object_t::STR,   "STR"},
		{object_t::INT,   "INT"},
		{object_t::FLOAT, "FLOAT"},
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
	static std::initializer_list <std::pair<client_auth, std::string>> auths {
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

// extract an object from part of a message
template<object_t obj>
inline concrete_t<obj> extract(const std::string& msg, size_t& pos);

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

// extract exactly a single string from the rest of a message
// throw if there is more than one string
static inline std::string extract_single_string(const std::string& msg, size_t& pos)
{
	std::string word = extract<object_t::STR>(msg, pos);
	ensure_end(msg, pos);
	return word;
}

// extract exactly two strings from the rest of a message
// throw if there are more than two strings
static inline std::pair<std::string, std::string> extract_two_strings(const std::string& msg, size_t& pos)
{
	std::string word1 = extract<object_t::STR>(msg, pos);
	std::string word2 = extract<object_t::STR>(msg, pos);
	ensure_end(msg, pos);
	return {word1, word2};
}

// extract exactly a single int64_t from the rest of a message
// throw if there is more than one int64_t
static inline int64_t extract_single_int(const std::string& msg, size_t& pos)
{
	int64_t value = extract<object_t::INT>(msg, pos);
	ensure_end(msg, pos);
	return value;
}

std::string RequestServer::prepare(const std::string &msg) {
	std::string ret {};
	ret.reserve(msg.size() + 10);
	ret += type_to_string<std::string>::value;

	ret += "\"";
	for (char c : msg) {
		if (c == '"')
			ret += "\\\"";
		else
			ret += c;
	}
	ret += "\"";

	return ret;
}

void RequestServer::handle(const std::string& msg, const Client& client) {
	try{
		size_t pos = 0;
		operation_t op = extract_operation(msg, pos);

		if (not client.has_perm(op))
			return send(client, server_constants::denied);

		switch (op) {
			case operation_t::GET:
			{
				request_get(client, extract_single_string(msg, pos));
				break;
			}
			case operation_t::SET:
			{
				dispatch_set(client, msg, pos);
				break;
			}
			case operation_t::DEL:
			{
				request_del(client, extract_single_string(msg, pos));
				break;
			}
			case operation_t::SWITCH_DB:
			{
				request_switch_db(client, extract_single_int(msg, pos));
				break;
			}
			case operation_t::ADD_USER:
			{
				auto [username, password] = extract_two_strings(msg, pos);
				request_add_user(client, username, password);
				break;
			}
			case operation_t::EDIT_USER:
			{
				auto username = extract<object_t::STR>(msg, pos);
				auto auth = extract_client_auth(msg, pos);
				ensure_end(msg, pos);
				request_edit_user(client, username, auth);
				break;
			}
			case operation_t::DEL_USER:
			{
				request_del_user(client, extract_single_string(msg, pos));
				break;
			}
			case operation_t::AUTH:
			{
				auto [username, password] = extract_two_strings(msg, pos);
				request_auth(client, username, password);
				break;
			}
			case operation_t::CHANGE_PASSWORD:
			{
				request_change_password(client, extract_single_string(msg, pos));
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
	catch (const InvalidRequest& e) {
		std::string err {server_constants::error};
		err += ": ";
		err += e.what();
		send(client, err);
	}
}

void RequestServer::dispatch_set(const Client &client, const std::string &msg, size_t &pos) {
	object_t obj = extract_object_t(msg, pos);
	std::string key = extract<object_t::STR>(msg, pos);
	switch (obj) {
		case object_t::STR:{
			auto value {extract<object_t::STR>(msg, pos)};
			ensure_end(msg, pos);
			request_set(client, key, value);
			break;
		}
		case object_t::INT:{
			auto value {extract<object_t::INT>(msg, pos)};
			ensure_end(msg, pos);
			request_set(client, key, value);
			break;
		}
		case object_t::FLOAT:{
			auto value {extract<object_t::FLOAT>(msg, pos)};
			ensure_end(msg, pos);
			request_set(client, key, value);
			break;
		}
	}
}

} // namespace vanity
