//
// Created by kingsli on 12/21/23.
//

#include <limits>

#include "client/operations.h"
#include "request.h"


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
// idx is assumed to be non-null
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

	struct Range_chk {
		static bool S_chk(TRet, std::false_type) {
			return false;
		}

		// only called when Ret is int
		static bool S_chk(TRet val, std::true_type) {
			static constexpr auto min = std::numeric_limits<int>::min();
			static constexpr auto max = std::numeric_limits<int>::max();
			return val < TRet(min) || val > TRet(max);
		}
	};

	Ret ret;
	char* end_ptr;
	const TRet tmp = func(str, &end_ptr, base...);

	if (end_ptr == str)
		std::__throw_invalid_argument(name);
	else if (errno == ERANGE || Range_chk::S_chk(tmp, std::is_same<Ret, int>{}))
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

inline const char& Request::current() const {
	return msg[pos];
}

inline bool Request::not_end() const {
	return pos < msg.size();
}

inline bool Request::end() const {
	return pos >= msg.size();
}

inline size_t Request::operator++() {
	return ++pos;
}

inline size_t Request::operator+=(size_t n) {
	return pos += n;
}

inline bool Request::compare(const std::string &str) const {
	return msg.compare(pos, str.size(), str) == 0;
}

inline bool Request::has_up_to(size_t n) const {
	return pos + n <= msg.size();
}

inline void Request::expect(char c, const char *err) {
	if (current() != c)
		throw InvalidRequest(err);
	++*this;
}

inline void Request::skip_whitespace() {
	while (not_end() && isspace(current()))
		++*this;
}

inline void Request::ensure_end() {
	skip_whitespace();
	if (not_end())
		throw InvalidRequest("unexpected character at end of message");
}

void Request::ensure_end_if(bool condition) {
	if (condition)
		ensure_end();
}

inline void Request::ensure_not_end() {
	skip_whitespace();
	if (end())
		throw InvalidRequest("unexpected end of message");
}

Request::Request(const std::string &msg) : msg(msg) {}

std::string Request::format() const {
	return msg + " at " + std::to_string(pos);
}

size_t Request::index() const {
	return pos;
}

std::string_view Request::view(size_t start, size_t end) const {
	if (start > end)
		throw std::out_of_range("start index greater than end index");

	if (end > msg.size())
		throw std::out_of_range("end index out of range");

	return {msg.data() + start, end - start};
}

operation_t Request::get_operation() {
	skip_whitespace();
	for (const auto &[op, str]: OPERATION_T_STRINGS)
		if (compare(str)) {
			*this += str.size();
			return op;
		}

	throw InvalidRequest("invalid operation");
}

operation_t Request::peek_operation() {
	skip_whitespace();
	for (const auto &[op, str]: OPERATION_T_STRINGS)
		if (compare(str))
			return op;

	throw InvalidRequest("invalid operation");
}

object_t Request::get_object_t() {
	skip_whitespace();
	expect(':', "expected object type");

	skip_whitespace();
	for (const auto &[obj, str]: OBJECT_T_STRINGS)
		if (compare(str)) {
			*this += str.size();
			return obj;
		}

	throw InvalidRequest("invalid object type");
}

client_auth Request::get_client_auth() {
	skip_whitespace();
	for (const auto &[auth, str]: CLIENT_AUTH_STRINGS)
		if (compare(str)) {
			*this += str.size();
			return auth;
		}

	throw InvalidRequest("invalid auth level");
}

size_t Request::get_len() {
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

int64_t Request::get_int() {
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

double Request::get_float() {
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

std::string Request::get_str() {
	size_t len = get_len();
	if (not has_up_to(len))
		throw InvalidRequest("string too short");

	auto ret = msg.substr(pos, len);
	*this += len;
	return ret;
}

std::vector<std::string> Request::get_arr() {
	size_t len = get_len();
	expect('[', "array not opened with bracket");

	std::vector<std::string> arr;
	arr.reserve(len);
	for (size_t i = 0; i < len; ++i)
		arr.emplace_back(get<object_t::STR>());

	expect(']', "array not closed with bracket");
	return arr;
}

std::list<std::string> Request::get_list() {
	size_t len = get_len();
	expect('[', "list not opened with bracket");

	std::list<std::string> list;
	for (size_t i = 0; i < len; ++i)
		list.emplace_back(get<object_t::STR>());

	expect(']', "list not closed with bracket");
	return list;
}

std::unordered_set<std::string> Request::get_set() {
	size_t len = get_len();
	expect('{', "set not opened with '{' bracket");

	std::unordered_set<std::string> set;
	for (size_t i = 0; i < len; ++i)
		set.emplace(get<object_t::STR>());

	expect('}', "set not closed with '}' bracket");
	return set;
}

std::unordered_map<std::string, std::string> Request::get_hash() {
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
