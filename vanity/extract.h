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
#include "request.h"


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

// increment request until it is not a whitespace
static inline void skip_whitespace(Request& request)
{
	while (request.not_end() and isspace(request.current()))
		++request;
}

// ensure we are at the end of the message
static inline void ensure_end(Request& request)
{
	skip_whitespace(request);
	if (request.not_end())
		throw InvalidRequest("unexpected character at end of message");
}

// ensure we are at the end of the message if condition is true
static inline void ensure_end_if(Request& request, bool condition)
{
	if (condition)
		ensure_end(request);
}

// ensure we are not at the end of the message
static inline void ensure_not_end(Request& request)
{
	skip_whitespace(request);
	if (request.end())
		throw InvalidRequest("unexpected end of message");
}

// extract the operation from the message
static inline operation_t extract_operation(Request& request)
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

			{operation_t::TRANSACT_BEGIN,  "TRANSACT_BEGIN"},
			{operation_t::TRANSACT_COMMIT, "TRANSACT_COMMIT"},
			{operation_t::TRANSACT_DISCARD,"TRANSACT_DISCARD"},

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

	skip_whitespace(request);
	for (const auto& [op, str] : operations) {
		if (request.compare(str)) {
			request += str.size();
			return op;
		}
	}
	throw InvalidRequest("invalid operation");
}

// extract the object type from the message
// currently unused
static inline object_t extract_object_t(Request& request)
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

	skip_whitespace(request);
	if (request.current() != object_separator) {
		throw InvalidRequest("expected object type");
	}
	++request;

	skip_whitespace(request);
	for (const auto& [obj, str] : objects) {
		if (request.compare(str)) {
			request += str.size();
			return obj;
		}
	}
	throw InvalidRequest("invalid object type");
}

// extract thea client auth level from the message
static inline client_auth extract_client_auth(Request& request)
{
	static const std::initializer_list <std::pair<client_auth, std::string>> auths {
			{client_auth::UNKNOWN,     "UNKNOWN"},
			{client_auth::USER,        "USER"},
			{client_auth::ADMIN,       "ADMIN"},
			{client_auth::PEER,        "PEER"},
	};

	skip_whitespace(request);
	for (const auto& [auth, str] : auths) {
		if (request.compare(str)) {
			request += str.size();
			return auth;
		}
	}
	throw InvalidRequest("invalid auth level");
}

// extract a (len) from part of a message
inline size_t extract_len(Request& request)
{
	ensure_not_end(request);
	if (request.current() != '(') {
		throw InvalidRequest("expected length specifier");
	}
	++request;

	try{
		size_t count = 0;
		size_t len = std::stoull(request.substr(), &count);
		request += count;
		if (request.current() != ')') {
			throw InvalidRequest("expected length specifier");
		}
		++request;
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
static inline concrete_t<Args...> extract(Request& request)
{
	return {extract<Args>(request)...};
}

// extract exactly the given object types from the rest of a message
// throw if expect_end is true and there are more or less than the given object types
template<object_t ...Args>
static inline concrete_t<Args...> extract_exact(Request& request, bool expect_end)
{
	auto ret = extract<Args...>(request);
	ensure_end_if(request, expect_end);
	return ret;
}

// extract an int64_t from part of a message
template<>
inline int64_t extract<object_t::INT>(Request& request)
{
	ensure_not_end(request);
	try{
		size_t count = 0;
		auto ret{std::stoll(request.substr(), &count)};
		request += count;
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
inline double extract<object_t::FLOAT>(Request& request)
{
	ensure_not_end(request);
	try{
		size_t count = 0;
		auto ret{std::stod(request.substr(), &count)};
		request += count;
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
inline std::string extract<object_t::STR>(Request& request)
{
	size_t len = extract_len(request);
	if (not request.has_up_to(len))
		throw InvalidRequest("string length mismatch");

	std::string ret = request.substr(len);
	request += len;
	return ret;
}

// extract a vector of strings from part of a message
template<>
inline std::vector<std::string> extract<object_t::ARR>(Request& request)
{
	size_t len = extract_len(request);
	if (request.current() != '[')
		throw InvalidRequest("array not opened with bracket");
	++request;

	std::vector<std::string> arr;
	arr.reserve(len);
	for (size_t i = 0; i < len; ++i)
		arr.emplace_back(extract<object_t::STR>(request));

	if (request.current() != ']')
		throw InvalidRequest("array not closed with bracket");
	++request;

	return arr;
}

// extract a list of strings from part of a message
template<>
inline std::list<std::string> extract<object_t::LIST>(Request& request)
{
	size_t len = extract_len(request);
	if (request.current() != '[')
		throw InvalidRequest("list not opened with bracket");
	++request;

	std::list<std::string> list;
	for (size_t i = 0; i < len; ++i)
		list.emplace_back(extract<object_t::STR>(request));

	if (request.current() != ']')
		throw InvalidRequest("list not closed with bracket");
	++request;

	return list;
}

// extract a set of strings from part of a message
template<>
inline std::unordered_set<std::string> extract<object_t::SET>(Request& request)
{
	size_t len = extract_len(request);
	if (request.current() != '{')
		throw InvalidRequest("set not opened with '{' bracket");
	++request;

	std::unordered_set<std::string> set;
	for (size_t i = 0; i < len; ++i)
		set.emplace(extract<object_t::STR>(request));

	if (request.current() != '}')
		throw InvalidRequest("set not closed with '}' bracket");
	++request;

	return set;
}

// extract a hash of strings from part of a message
template<>
inline std::unordered_map<std::string, std::string> extract<object_t::HASH>(Request& request)
{
	size_t len = extract_len(request);
	if (request.current() != '{')
		throw InvalidRequest("hash not opened with '{' bracket");
	++request;

	std::unordered_map<std::string, std::string> hash;
	for (size_t i = 0; i < len; ++i) {
		auto key = extract<object_t::STR>(request);
		auto value = extract<object_t::STR>(request);
		hash.emplace(std::move(key), std::move(value));
	}

	if (request.current() != '}')
		throw InvalidRequest("hash not closed with '}' bracket");
	++request;

	return hash;
}

} // namespace vanity

#endif //VANITY_EXTRACT_H
