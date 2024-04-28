//
// Created by kingsli on 4/28/24.
//

#ifndef VANITY_EXTRACTABLE_H
#define VANITY_EXTRACTABLE_H

#include "object_t.h"

namespace vanity {

/*
 * An Extractable lets us extract values from a string
 */
class Extractable
{
protected:
	// the string
	const std::string& m_msg;

	// the current position in the string
	size_t m_pos = 0;

	// the current character in the string
	// shorthand for m_msg[m_pos]
	const char& current() const;

	// returns true if the string has been fully processed
	// shorthand for m_pos >= m_msg.size()
	bool end() const;

	// increments the current position in the string
	// shorthand for ++m_pos
	size_t operator++();

	// increments the current position in the string
	// shorthand for m_pos += n
	size_t operator+=(size_t n);

	// compares from the current position in the string to another string
	// shorthand for m_msg.compare(m_pos, str.size(), str) == 0
	bool compare(const std::string& str) const;

	// checks if there are up to n characters left in the string
	// shorthand for m_pos + n <= m_msg.size()
	bool has_up_to(size_t n) const;

	// expects the current() character to be c and increments this
	// or throw an InvalidExtractable with err
	void expect(char c, const char * err);

	// increment until current() is not whitespace
	// or the end of the string is reached
	void skip_whitespace();

	// ensure we are at the end of the string
	void ensure_end();

	// if condition is true, ensure we are at the end of the string
	void ensure_end_if(bool condition);

	// ensure we are not at the end of the string
	void ensure_not_end();

	// extract an integer from the extractable
	int64_t get_int();

	// extract a float from the extractable
	double get_float();

	// extract a string from the extractable
	std::string get_str();

	// extract an array from the extractable
	std::vector<std::string> get_arr();

	// extract a list from the extractable
	std::list<std::string> get_list();

	// extract a set from the extractable
	std::unordered_set<std::string> get_set();

	// extract a hash from the extractable
	std::unordered_map<std::string, std::string> get_hash();

	// extract a client auth from the extractable
	client_auth get_client_auth();

	template<class T>
	using list_of_pairs = std::initializer_list<std::pair<T, std::string>>;

	// extract a value from a list of pairs
	// by comparing the strings in the pairs
	// in order to the extractable and returning the value
	// of the first match or throw an InvalidRequest with err
	template<class T>
	T get_from_list(const list_of_pairs<T>& list, const char* err);

	// peek a value from a list of pairs
	// same as get_from_list but doesn't advance the position
	template<class T>
	T peek_from_list(const list_of_pairs<T>& list, const char* err);

public:
	// extract a (len) from part of an extractable
	size_t get_len();

	// extract objects from an extractable
	template<object_t ...Args>
	concrete_t<Args...> get() {
		return {get<Args>()...};
	}

	// extract exactly the given object types from the extractable
	// throw if end is true and there are more or less than the given object types
	template<object_t ...Args>
	concrete_t<Args...> get_exact(bool end) {
		auto ret = get<Args...>();
		ensure_end_if(end);
		return ret;
	}

	// create an Extractable with a string
	explicit Extractable(const std::string& msg, size_t pos = 0);

	// copy constructor
	Extractable(const Extractable& other) = default;

	// move constructor
	Extractable(Extractable&& other) = default;
};

template<>
inline int64_t Extractable::get<object_t::INT>() {
	return get_int();
}

template<>
inline double Extractable::get<object_t::FLOAT>() {
	return get_float();
}

template<>
inline std::string Extractable::get<object_t::STR>() {
	return get_str();
}

template<>
inline std::vector<std::string> Extractable::get<object_t::ARR>() {
	return get_arr();
}

template<>
inline std::list<std::string> Extractable::get<object_t::LIST>() {
	return get_list();
}

template<>
inline std::unordered_set<std::string> Extractable::get<object_t::SET>() {
	return get_set();
}

template<>
inline std::unordered_map<std::string, std::string> Extractable::get<object_t::HASH>() {
	return get_hash();
}

template<>
inline client_auth Extractable::get<object_t::CLIENT_AUTH>() {
	return get_client_auth();
}

} // namespace vanity

#endif //VANITY_EXTRACTABLE_H
