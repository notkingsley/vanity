//
// Created by kingsli on 12/21/23.
//

#include "client/operations.h"
#include "request.h"


namespace vanity {

inline char Request::current() const {
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

inline std::string Request::substr() const {
	return msg.substr(pos);
}

inline std::string Request::substr(size_t n) const {
	return msg.substr(pos, n);
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
		size_t len = std::stoull(substr(), &count);
		*this += count;

		expect(')', "expected length specifier");
		return len;
	}
	catch (const std::out_of_range &e) {
		throw InvalidRequest("length out of range");
	}
	catch (const std::invalid_argument &e) {
		throw InvalidRequest("invalid length");
	}
}

int64_t Request::get_int() {
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

double Request::get_float() {
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

std::string Request::get_str() {
	size_t len = get_len();
	if (not has_up_to(len))
		throw InvalidRequest("string too short");

	std::string ret = substr(len);
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
