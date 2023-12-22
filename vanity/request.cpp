//
// Created by kingsli on 12/21/23.
//

#include "request.h"

namespace vanity {

void Request::skip_whitespace() {
	while (not_end() && isspace(current()))
		++*this;
}

void Request::ensure_end() {
	skip_whitespace();
	if (not_end())
		throw InvalidRequest("unexpected character at end of message");
}

void Request::ensure_end_if(bool condition) {
	if (condition)
		ensure_end();
}

void Request::ensure_not_end() {
	skip_whitespace();
	if (end())
		throw InvalidRequest("unexpected end of message");
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

} // namespace vanity