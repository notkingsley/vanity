//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_REQUEST_H
#define VANITY_REQUEST_H

#include <list>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "exceptions.h"
#include "client/permissions.h"


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

const std::initializer_list<std::pair<object_t, std::string>> OBJECT_T_STRINGS {
	{object_t::STR,   "STR"},
	{object_t::INT,   "INT"},
	{object_t::FLOAT, "FLOAT"},
	{object_t::ARR,   "ARR"},
	{object_t::LIST,  "LIST"},
	{object_t::SET,   "SET"},
	{object_t::HASH,  "HASH"},
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

/*
 * A Request is a class that holds information
 * about a request string currently being processed
 */
class Request
{
private:
	// the request string
	const std::string& msg;

	// the current position in the request string
	size_t pos = 0;

	// the current character in the request string
	// shorthand for msg[pos]
	char current() const {
		return msg[pos];
	}

	// returns true if the request string has not been fully processed
	// shorthand for pos < msg.size()
	bool not_end() const {
		return pos < msg.size();
	}

	// returns true if the request string has been fully processed
	// shorthand for pos >= msg.size()
	bool end() const {
		return pos >= msg.size();
	}

	// increments the current position in the request string
	// shorthand for ++pos
	size_t operator++() {
		return ++pos;
	}

	// increments the current position in the request string
	// shorthand for pos += n
	size_t operator+=(size_t n) {
		return pos += n;
	}

	// compares from the current position in the request string to another string
	// shorthand for msg.compare(pos, str.size(), str) == 0
	bool compare(const std::string& str) const {
		return msg.compare(pos, str.size(), str) == 0;

	}

	// returns a substring of the request string from the current position
	// shorthand for msg.substr(pos)
	std::string substr() const {
		return msg.substr(pos);
	}

	// returns a substring of the request string from the current position
	// shorthand for msg.substr(pos, n)
	std::string substr(size_t n) const {
		return msg.substr(pos, n);
	}

	// checks if there are up to n characters left in the request string
	// shorthand for pos + n <= msg.size()
	bool has_up_to(size_t n) const {
		return pos + n <= msg.size();
	}

	// expects the current() character to be c and increments this
	// or throw an InvalidRequest with err
	void expect(char c, const char * err) {
		if (current() != c)
			throw InvalidRequest(err);
		++*this;
	}

	// increment until current() is not whitespace
	// or the end of the request string is reached
	void skip_whitespace();

public:
	// create a Request with a request string
	explicit Request(const std::string& msg) : msg(msg) {}

	// formats the request string and position logging
	std::string format() const {
		return msg + " at " + std::to_string(pos);
	}

	// ensure we are at the end of the request string
	void ensure_end();

	// if condition is true, ensure we are at the end of the request string
	void ensure_end_if(bool condition);

	// ensure we are not at the end of the request string
	void ensure_not_end();

	// extract the operation type from the request
	operation_t get_operation();

	// like get_operation, but doesn't advance the position
	operation_t peek_operation();

	// extract the object type from the request
	// currently unused
	object_t get_object_t();

	// extract the client auth level from the request
	client_auth get_client_auth();

	// extract a (len) from part of a request
	size_t get_len();

	// extract an object from a request
	template<object_t ...Args>
	concrete_t<Args...> get() {
		return {get<Args>()...};
	}

	// extract exactly the given object types from the request
	// throw if end is true and there are more or less than the given object types
	template<object_t ...Args>
	concrete_t<Args...> get_exact(bool end) {
		auto ret = get<Args...>();
		ensure_end_if(end);
		return ret;
	}

	// get thd index of the current position in the request string
	size_t index() const {
		return pos;
	}

	// get a view of the request string between two indices
	std::string_view view(size_t start, size_t end) const {
		if (start > end)
			throw std::out_of_range("start index greater than end index");

		if (end > msg.size())
			throw std::out_of_range("end index out of range");

		return {msg.data() + start, end - start};
	}
};

template<>
inline int64_t Request::get<object_t::INT>() {
	ensure_not_end();
	try {
		size_t count = 0;
		auto ret{std::stoll(substr(), &count)};
		*this += count;
		return ret;
	}
	catch (const std::out_of_range &e) {
		throw InvalidRequest("integer out of range");
	}
	catch (const std::invalid_argument &e) {
		throw InvalidRequest("invalid integer");
	}
}

template<>
inline double Request::get<object_t::FLOAT>() {
	ensure_not_end();
	try {
		size_t count = 0;
		auto ret{std::stod(substr(), &count)};
		*this += count;
		return ret;
	}
	catch (const std::out_of_range &e) {
		throw InvalidRequest("float out of range");
	}
	catch (const std::invalid_argument &e) {
		throw InvalidRequest("invalid float");
	}
}

template<>
inline std::string Request::get<object_t::STR>() {
	size_t len = get_len();
	if (not has_up_to(len))
		throw InvalidRequest("string too short");

	std::string ret = substr(len);
	*this += len;
	return ret;
}

template<>
inline std::vector<std::string> Request::get<object_t::ARR>() {
	size_t len = get_len();
	expect('[', "array not opened with bracket");

	std::vector<std::string> arr;
	arr.reserve(len);
	for (size_t i = 0; i < len; ++i)
		arr.emplace_back(get<object_t::STR>());

	expect(']', "array not closed with bracket");
	return arr;
}

template<>
inline std::list<std::string> Request::get<object_t::LIST>() {
	size_t len = get_len();
	expect('[', "list not opened with bracket");

	std::list<std::string> list;
	for (size_t i = 0; i < len; ++i)
		list.emplace_back(get<object_t::STR>());

	expect(']', "list not closed with bracket");
	return list;
}

template<>
inline std::unordered_set<std::string> Request::get<object_t::SET>() {
	size_t len = get_len();
	expect('{', "set not opened with '{' bracket");

	std::unordered_set<std::string> set;
	for (size_t i = 0; i < len; ++i)
		set.emplace(get<object_t::STR>());

	expect('}', "set not closed with '}' bracket");
	return set;
}

template<>
inline std::unordered_map<std::string, std::string> Request::get<object_t::HASH>() {
	size_t len = get_len();
	expect('{', "hash not opened with '{' bracket");

	std::unordered_map<std::string, std::string> hash;
	for (size_t i = 0; i < len; ++i) {
		auto key = get<object_t::STR>();
		auto value = get<object_t::STR>();
		hash.emplace(std::move(key), std::move(value));
	}

	expect('}', "hash not closed with '}' bracket");
	return hash;
}

} // namespace vanity

#endif //VANITY_REQUEST_H
