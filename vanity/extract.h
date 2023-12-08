//
// Created by kingsli on 11/28/23.
//

#ifndef VANITY_EXTRACT_H
#define VANITY_EXTRACT_H

#include <list>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "exceptions.h"
#include "permissions.h"


namespace vanity {

// all object types
enum class object_t{
	STR,
	INT,
	FLOAT,
	ARR,
	LIST,
	SET,
	HASH,
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

template<>
struct concrete_traits<object_t::LIST> {
	using type = std::list<std::string>;
};

template<>
struct concrete_traits<object_t::SET> {
	using type = std::unordered_set<std::string>;
};

template<>
struct concrete_traits<object_t::HASH> {
	using type = std::unordered_map<std::string, std::string>;
};

template <object_t ...Args>
struct concrete {
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
			{operation_t::EXIT,            "EXIT"},
			{operation_t::TERMINATE,       "TERMINATE"},
			{operation_t::PING,            "PING"},
			{operation_t::PIPE,            "PIPE"},

			{operation_t::AUTH,            "AUTH"},
			{operation_t::ADD_USER,        "ADD_USER"},
			{operation_t::EDIT_USER,       "EDIT_USER"},
			{operation_t::DEL_USER,        "DEL_USER"},
			{operation_t::CHANGE_PASSWORD, "CHANGE_PASSWORD"},

			{operation_t::SWITCH_DB,       "SWITCH_DB"},
			{operation_t::PERSIST,         "PERSIST"},

			{operation_t::DEL,             "DEL"},
			{operation_t::TYPE,            "TYPE"},
			{operation_t::EXISTS,          "EXISTS"},
			{operation_t::RESET,           "RESET"},
			{operation_t::KEYS,            "KEYS"},
			{operation_t::COPY_TO_DB,      "COPY_TO_DB"},
			{operation_t::MOVE_TO_DB,      "MOVE_TO_DB"},
			{operation_t::COPY_TO,         "COPY_TO"},
			{operation_t::MOVE_TO,         "MOVE_TO"},

			{operation_t::SET_EXPIRY,      "SET_EXPIRY"},
			{operation_t::GET_EXPIRY,      "GET_EXPIRY"},
			{operation_t::CLEAR_EXPIRY,    "CLEAR_EXPIRY"},

			{operation_t::GET,             "GET"},
			{operation_t::STR_SET,         "STR_SET"},
			{operation_t::INT_SET,         "INT_SET"},
			{operation_t::FLOAT_SET,       "FLOAT_SET"},
			{operation_t::INCR_INT,        "INCR_INT"},
			{operation_t::INCR_FLOAT,      "INCR_FLOAT"},
			{operation_t::STR_LEN,         "STR_LEN"},
			{operation_t::MANY_GET,        "MANY_GET"},

			{operation_t::LIST_LEN,        "LIST_LEN"},
			{operation_t::LIST_GET,        "LIST_GET"},
			{operation_t::LIST_SET,        "LIST_SET"},
			{operation_t::LIST_PUSH_LEFT,  "LIST_PUSH_LEFT"},
			{operation_t::LIST_PUSH_RIGHT, "LIST_PUSH_RIGHT"},
			{operation_t::LIST_POP_LEFT,   "LIST_POP_LEFT"},
			{operation_t::LIST_POP_RIGHT,  "LIST_POP_RIGHT"},
			{operation_t::LIST_RANGE,      "LIST_RANGE"},
			{operation_t::LIST_TRIM,       "LIST_TRIM"},
			{operation_t::LIST_REMOVE,     "LIST_REMOVE"},

			{operation_t::SET_ADD,         "SET_ADD"},
			{operation_t::SET_ALL,         "SET_ALL"},
			{operation_t::SET_REMOVE,      "SET_REMOVE"},
			{operation_t::SET_DISCARD,     "SET_DISCARD"},
			{operation_t::SET_LEN,         "SET_LEN"},
			{operation_t::SET_CONTAINS,    "SET_CONTAINS"},
			{operation_t::SET_MOVE,        "SET_MOVE"},
			{operation_t::SET_UNION_INTO,  "SET_UNION_INTO"},
			{operation_t::SET_UNION_LEN,   "SET_UNION_LEN"},
			{operation_t::SET_UNION,       "SET_UNION"},
			{operation_t::SET_INTERSECT_INTO,"SET_INTERSECT_INTO"},
			{operation_t::SET_INTERSECT_LEN,"SET_INTERSECT_LEN"},
			{operation_t::SET_INTERSECT,   "SET_INTERSECT"},
			{operation_t::SET_DIFF_INTO,   "SET_DIFF_INTO"},
			{operation_t::SET_DIFF_LEN,    "SET_DIFF_LEN"},
			{operation_t::SET_DIFF,        "SET_DIFF"},

			{operation_t::HASH_SET,        "HASH_SET"},
			{operation_t::HASH_ALL,        "HASH_ALL"},
			{operation_t::HASH_GET,        "HASH_GET"},
			{operation_t::HASH_CONTAINS,   "HASH_CONTAINS"},
			{operation_t::HASH_LEN,        "HASH_LEN"},
			{operation_t::HASH_KEY_LEN,    "HASH_KEY_LEN"},
			{operation_t::HASH_REMOVE,     "HASH_REMOVE"},
			{operation_t::HASH_KEYS,       "HASH_KEYS"},
			{operation_t::HASH_VALUES,     "HASH_VALUES"},
			{operation_t::HASH_UPDATE,     "HASH_UPDATE"},
			{operation_t::HASH_MANY_GET,   "HASH_MANY_GET"},
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
// currently unused
static inline object_t extract_object_t(const std::string& msg, size_t& pos)
{
	static const char object_separator = ':';
	static const std::initializer_list <std::pair<object_t, std::string>> objects {
			{object_t::STR,   "STR"},
			{object_t::INT,   "INT"},
			{object_t::FLOAT, "FLOAT"},
			{object_t::ARR,   "ARR"},
			{object_t::LIST,  "LIST"},
			{object_t::SET,   "SET"},
			{object_t::HASH,  "HASH"},
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
inline size_t extract_len(const std::string& msg, size_t& pos)
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
// throw if expect_end is true and there are more or less than the given object types
template<object_t ...Args>
static inline concrete_t<Args...> extract_exact(const std::string& msg, size_t& pos, bool expect_end)
{
	auto ret = extract<Args...>(msg, pos);
	if (expect_end)
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
	size_t len = extract_len(msg, pos);
	if (pos + len > msg.size())
		throw InvalidRequest("string length mismatch");

	std::string ret = msg.substr(pos, len);
	pos += len;
	return ret;
}

// extract a vector of strings from part of a message
template<>
inline std::vector<std::string> extract<object_t::ARR>(const std::string& msg, size_t& pos)
{
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

// extract a list of strings from part of a message
template<>
inline std::list<std::string> extract<object_t::LIST>(const std::string& msg, size_t& pos)
{
	size_t len = extract_len(msg, pos);
	if (msg[pos] != '[')
		throw InvalidRequest("list not opened with bracket");
	++pos;

	std::list<std::string> list;
	for (size_t i = 0; i < len; ++i)
		list.emplace_back(extract<object_t::STR>(msg, pos));

	if (msg[pos] != ']')
		throw InvalidRequest("list not closed with bracket");
	++pos;

	return list;
}

// extract a set of strings from part of a message
template<>
inline std::unordered_set<std::string> extract<object_t::SET>(const std::string& msg, size_t& pos)
{
	size_t len = extract_len(msg, pos);
	if (msg[pos] != '{')
		throw InvalidRequest("set not opened with '{' bracket");
	++pos;

	std::unordered_set<std::string> set;
	for (size_t i = 0; i < len; ++i)
		set.emplace(extract<object_t::STR>(msg, pos));

	if (msg[pos] != '}')
		throw InvalidRequest("set not closed with '}' bracket");
	++pos;

	return set;
}

// extract a hash of strings from part of a message
template<>
inline std::unordered_map<std::string, std::string> extract<object_t::HASH>(const std::string& msg, size_t& pos)
{
	size_t len = extract_len(msg, pos);
	if (msg[pos] != '{')
		throw InvalidRequest("hash not opened with '{' bracket");
	++pos;

	std::unordered_map<std::string, std::string> hash;
	for (size_t i = 0; i < len; ++i) {
		auto key = extract<object_t::STR>(msg, pos);
		auto value = extract<object_t::STR>(msg, pos);
		hash.emplace(std::move(key), std::move(value));
	}

	if (msg[pos] != '}')
		throw InvalidRequest("hash not closed with '}' bracket");
	++pos;

	return hash;
}

} // namespace vanity

#endif //VANITY_EXTRACT_H
