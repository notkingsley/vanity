//
// Created by kingsli on 4/28/24.
//

#include <limits>

#include "client/operations.h"
#include "exceptions.h"
#include "extractable.h"
#include "peer_request.h"
#include "post_request.h"


namespace vanity {

namespace {

// Helper for all the sto* functions.
//
// This is a simplified copy of the
// __gnu_cxx::__stoa conversion function.
//
// It is copied to avoid a compiler dependency
// this provides weaker guarantees and
// does NOT serve as a drop in replacement: for example
// idx cannot be null and the limits for int done
// with Range_chk are omitted since we are not using int
template<typename TRet, typename Ret = TRet, typename... Base>
Ret stoa(TRet (*func) (const char*, char**, Base...),
		 const char* name, const char* str, std::size_t* idx,
		 Base... base)
{
	struct Save_errno {
		Save_errno() : m_errno(errno) { errno = 0; }
		~Save_errno() { if (errno == 0) errno = m_errno; }
		int m_errno;
	} const save_errno;

	Ret ret;
	char* end_ptr;
	const TRet tmp = func(str, &end_ptr, base...);

	if (end_ptr == str)
		std::__throw_invalid_argument(name);
	else if (errno == ERANGE)
		std::__throw_out_of_range(name);
	else
		ret = tmp;

	*idx = end_ptr - str;
	return ret;
}

// selected rewrite of std::sto* functions
inline long long int
stoll(const char *str, size_t *idx) {
	return stoa(&std::strtoll, "stoll", str, idx, 10);
}

inline unsigned long long int
stoull(const char *str, size_t *idx) {
	return stoa(&std::strtoull, "stoull", str, idx, 10);
}

inline double
stod(const char* str, size_t* idx) {
	return stoa(&std::strtod, "stod", str, idx);
}

} // namespace

Extractable::Extractable(const std::string &msg, size_t pos) : m_msg{msg}, m_pos{pos} {}

inline const char& Extractable::current() const {
	return m_msg[m_pos];
}

inline bool Extractable::end() const {
	return m_pos >= m_msg.size();
}

inline size_t Extractable::operator++() {
	return ++m_pos;
}

inline size_t Extractable::operator+=(size_t n) {
	return m_pos += n;
}

inline bool Extractable::compare(const std::string &str) const {
	return m_msg.compare(m_pos, str.size(), str) == 0;
}

inline bool Extractable::has_up_to(size_t n) const {
	return m_pos + n <= m_msg.size();
}

inline void Extractable::expect(char c, const char *err) {
	if (current() != c)
		throw InvalidRequest(err);

	++*this;
}

inline void Extractable::skip_whitespace() {
	while (not end() && isspace(current()))
		++*this;
}

inline void Extractable::ensure_end() {
	skip_whitespace();
	if (not end())
		throw InvalidRequest("unexpected character at end of message");
}

void Extractable::ensure_end_if(bool condition) {
	if (condition)
		ensure_end();
}

inline void Extractable::ensure_not_end() {
	skip_whitespace();
	if (end())
		throw InvalidRequest("unexpected end of message");
}

client_auth Extractable::get_client_auth() {
	return get_from_list(CLIENT_AUTH_STRINGS, "invalid auth level");
}

size_t Extractable::get_len() {
	ensure_not_end();
	expect('(', "expected length specifier");

	try {
		size_t count = 0;
		auto len {stoull(&current(), &count)};
		*this += count;

		expect(')', "expected length specifier");
		return len;
	}
	catch (const std::out_of_range&) {
		throw InvalidRequest("length out of range");
	}
	catch (const std::invalid_argument&) {
		throw InvalidRequest("invalid length");
	}
}

int64_t Extractable::get_int() {
	ensure_not_end();
	try {
		size_t count = 0;
		auto ret {stoll(&current(), &count)};
		*this += count;
		return ret;
	}
	catch (const std::out_of_range&) {
		throw InvalidRequest("integer out of range");
	}
	catch (const std::invalid_argument&) {
		throw InvalidRequest("invalid integer");
	}
}

double Extractable::get_float() {
	ensure_not_end();
	try {
		size_t count = 0;
		auto ret {stod(&current(), &count)};
		*this += count;
		return ret;
	}
	catch (const std::out_of_range&) {
		throw InvalidRequest("float out of range");
	}
	catch (const std::invalid_argument&) {
		throw InvalidRequest("invalid float");
	}
}

std::string Extractable::get_str() {
	size_t len = get_len();
	if (not has_up_to(len))
		throw InvalidRequest("string too short");

	auto ret = m_msg.substr(m_pos, len);
	*this += len;
	return ret;
}

std::vector<std::string> Extractable::get_arr() {
	size_t len = get_len();
	expect('[', "array not opened with bracket");

	std::vector<std::string> arr;
	arr.reserve(len);
	for (size_t i = 0; i < len; ++i)
		arr.emplace_back(get_str());

	expect(']', "array not closed with bracket");
	return arr;
}

std::list<std::string> Extractable::get_list() {
	size_t len = get_len();
	expect('[', "list not opened with bracket");

	std::list<std::string> list;
	for (size_t i = 0; i < len; ++i)
		list.emplace_back(get_str());

	expect(']', "list not closed with bracket");
	return list;
}

std::unordered_set<std::string> Extractable::get_set() {
	size_t len = get_len();
	expect('{', "set not opened with '{' bracket");

	std::unordered_set<std::string> set;
	for (size_t i = 0; i < len; ++i)
		set.emplace(get_str());

	expect('}', "set not closed with '}' bracket");
	return set;
}

std::unordered_map<std::string, std::string> Extractable::get_hash() {
	size_t len = get_len();
	expect('{', "hash not opened with '{' bracket");

	std::unordered_map<std::string, std::string> hash;
	for (size_t i = 0; i < len; ++i) {
		auto key = get_str();
		auto value = get_str();
		hash.emplace(std::move(key), std::move(value));
	}

	expect('}', "hash not closed with '}' bracket");
	return hash;
}

template<class T>
T Extractable::peek_from_list(const Extractable::list_of_pairs<T> &list, const char *err) {
	skip_whitespace();
	for (const auto& [val, str]: list)
		if (compare(str))
			return val;

	throw InvalidRequest(err);
}

template<class T>
T Extractable::get_from_list(const list_of_pairs<T> &list, const char *err) {
	skip_whitespace();
	for (const auto& [val, str]: list)
		if (compare(str)) {
			*this += str.size();
			return val;
		}

	throw InvalidRequest(err);
}

// explicit instantiations
template object_t Extractable::get_from_list(const list_of_pairs<object_t>&, const char*);

template operation_t Extractable::get_from_list(const list_of_pairs<operation_t>&, const char*);

template peer_request_t Extractable::get_from_list(const list_of_pairs<peer_request_t>&, const char*);

template peer_op_t Extractable::get_from_list(const list_of_pairs<peer_op_t>&, const char*);

template operation_t Extractable::peek_from_list(const list_of_pairs<operation_t>&, const char*);

} // namespace vanity
