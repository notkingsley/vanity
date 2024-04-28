//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_REQUEST_H
#define VANITY_REQUEST_H

#include "exceptions.h"
#include "object_t.h"


namespace vanity {

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
	const char& current() const;

	// returns true if the request string has not been fully processed
	// shorthand for pos < msg.size()
	bool not_end() const;

	// returns true if the request string has been fully processed
	// shorthand for pos >= msg.size()
	bool end() const;

	// increments the current position in the request string
	// shorthand for ++pos
	size_t operator++();

	// increments the current position in the request string
	// shorthand for pos += n
	size_t operator+=(size_t n);

	// compares from the current position in the request string to another string
	// shorthand for msg.compare(pos, str.size(), str) == 0
	bool compare(const std::string& str) const;

	// checks if there are up to n characters left in the request string
	// shorthand for pos + n <= msg.size()
	bool has_up_to(size_t n) const;

	// expects the current() character to be c and increments this
	// or throw an InvalidRequest with err
	void expect(char c, const char * err);

	// increment until current() is not whitespace
	// or the end of the request string is reached
	void skip_whitespace();

	// ensure we are at the end of the request string
	void ensure_end();

	// if condition is true, ensure we are at the end of the request string
	void ensure_end_if(bool condition);

	// ensure we are not at the end of the request string
	void ensure_not_end();

	// extract an integer from the request
	int64_t get_int();

	// extract a float from the request
	double get_float();

	// extract a string from the request
	std::string get_str();

	// extract an array from the request
	std::vector<std::string> get_arr();

	// extract a list from the request
	std::list<std::string> get_list();

	// extract a set from the request
	std::unordered_set<std::string> get_set();

	// extract a hash from the request
	std::unordered_map<std::string, std::string> get_hash();

	// extract a client auth from the request
	client_auth get_client_auth();

	// extract the object type from the request
	// currently unused
	object_t get_object_t();

public:
	// create a Request with a request string
	explicit Request(const std::string& msg);

	// formats the request string and position for logging
	std::string format() const;

	// get the index of the current position in the request string
	size_t index() const;

	// get a view of the request string between two indices
	std::string_view view(size_t start, size_t end) const;

	// extract the operation type from the request
	operation_t get_operation();

	// like get_operation, but doesn't advance the position
	operation_t peek_operation();

	// extract a (len) from part of a request
	size_t get_len();

	// extract objects from a request
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
};

template<>
inline int64_t Request::get<object_t::INT>() {
	return get_int();
}

template<>
inline double Request::get<object_t::FLOAT>() {
	return get_float();
}

template<>
inline std::string Request::get<object_t::STR>() {
	return get_str();
}

template<>
inline std::vector<std::string> Request::get<object_t::ARR>() {
	return get_arr();
}

template<>
inline std::list<std::string> Request::get<object_t::LIST>() {
	return get_list();
}

template<>
inline std::unordered_set<std::string> Request::get<object_t::SET>() {
	return get_set();
}

template<>
inline std::unordered_map<std::string, std::string> Request::get<object_t::HASH>() {
	return get_hash();
}

template<>
inline client_auth Request::get<object_t::CLIENT_AUTH>() {
	return get_client_auth();
}

} // namespace vanity

#endif //VANITY_REQUEST_H
