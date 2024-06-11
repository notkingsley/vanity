//
// Created by kingsli on 10/26/23.
//

#ifndef VANITY_RESPONSE_H
#define VANITY_RESPONSE_H

#include <concepts>

#include "db/db/types.h"
#include "client/session_info.h"
#include "sendable.h"


namespace vanity {

using primary_serialize_type = db::db_data_type;

// an error_t is basically a wrapper for
// const char*s to be transmitted as error messages
struct error_t {
	const char* err;
};

// convert a type to a string
template <typename T>
struct type_to_string;

template <>
struct type_to_string<bool> {
	static constexpr const char* value = ":BOOL";
};

template <>
struct type_to_string<int64_t> {
	static constexpr const char* value = ":INT";
};

template <>
struct type_to_string<double> {
	static constexpr const char* value = ":FLOAT";
};

template <>
struct type_to_string<std::string> {
	static constexpr const char* value = ":STR";
};

template <>
struct type_to_string<std::monostate> {
	static constexpr const char* value = ":NULL";
};

template <>
struct type_to_string<std::vector<std::string>> {
	static constexpr const char* value = ":ARR";
};

template <typename T>
struct type_to_string<std::vector<T>> {
	static constexpr const char* value = ":TUPLE";
};

template <>
struct type_to_string<db::list_t> {
	static constexpr const char* value = ":LIST";
};

template <>
struct type_to_string<db::set_t> {
	static constexpr const char* value = ":SET";
};

template <>
struct type_to_string<db::hash_t> {
	static constexpr const char* value = ":HASH";
};

template <>
struct type_to_string<client_auth> {
	static constexpr const char* value = ":AUTH_LEVEL";
};

template<>
struct type_to_string<error_t> {
	static constexpr const char* value = ":ERR";
};

/*
 * A Response allows a dynamically efficient method to compose a response
 */
class Response : public Sendable
{
private:
	struct status_value {
		static constexpr char const* const ok = "OK";
		static constexpr char const* const null = "NULL";
		static constexpr char const* const pong = "PONG";
		static constexpr char const* const denied = "DENIED";
		static constexpr char const* const queued = "QUEUED";
		static constexpr char const* const bad_state = "BAD_STATE";
		static constexpr char const* const error = "ERROR";
		static constexpr char const* const bad_type = "BAD_TYPE";
		static constexpr char const* const bad_request = "BAD_REQUEST";
		static constexpr char const* const internal_error = "INTERNAL_ERROR";
		static constexpr char const* const async = "ASYNC";
	};

	// type-dependent std::false_type to delay evaluation till instantiation
	template <typename T>
	struct false_type : std::false_type {};

public:
	enum Status {
		ok,
		null,
		pong,
		denied,
		queued,
		bad_state,
		error,
		bad_type,
		bad_request,
		internal_error,
		async,
	};

	// return this as a rvalue
	Response&& move();

	// add a status to the response
	Response& operator<<(Status status);

	// add a message to the response
	Response& operator<<(const std::string& data);

	// add a message to the response
	Response& operator<<(const char* data);

	// add a message to the response
	Response& operator<<(char data);

	// serialize a bool to a Response
	Response& serialize(bool data);

	// serialize a string to a Response
	Response& serialize(const std::string &data);

	// serialize an int64_t to a Response
	Response& serialize(int64_t data);

	// serialize a double to a Response
	Response& serialize(size_t data);

	// serialize a double to a Response
	Response& serialize(double data);

	// serialize a list of strings to a Response
	Response& serialize(const std::list<std::string>& data);

	// serialize a set of strings to a Response
	Response& serialize(const std::unordered_set<std::string>& data);

	// serialize a hash of strings to a Response
	Response& serialize(const std::unordered_map<std::string, std::string>& data);

	// serialize the database's primary type to a Response
	Response& serialize(const primary_serialize_type& data);

	// serialize a client_auth to a Response
	Response& serialize(client_auth data);

	// serialize an error_t to a Response
	Response& serialize(error_t data);

	// serialize nothing to a Response (does nothing)
	Response& serialize();

	// do not serialize const char* directly
	// call serialize_string_body to add a string,
	// operator<< for raw data,
	// or with an error_t for errors
	Response& serialize(const char*) = delete;

	// serialize something to a Response
	template<typename T>
	Response& serialize(const T& data) {
		static_assert(false_type<T>::value, "cannot serialize this type");
		return *this;
	}

	// serialize an optional object to a Response
	template<typename T>
	Response& serialize(const std::optional<T>& data) {
		if (data.has_value())
			return serialize(data.value());
		else
			return serialize_type<std::monostate>();
	}

	// serialize a vector to a Response
	template<typename T>
	Response& serialize(const std::vector<T>& data) {
		// gcc bug prevents in-class specialization, so we have to do this
		if constexpr (std::is_same_v<T, std::string>)
			return serialize_arr(data);
		else
			return serialize_tuple(data);
	}

	// serialize a vector of strings as an ARR to a Response
	Response& serialize_arr(const std::vector<std::string>& data) {
		serialize_type<std::vector<std::string>>();
		serialize_length(data.size());

		*this << '[';
		for (const auto& s : data)
			serialize_string_body(s);
		return *this << ']';
	}

	// serialize an arbitrary vector as a TUPLE to a Response
	template<typename T>
	Response& serialize_tuple(const std::vector<T>& data) {
		static_assert(!std::is_same_v<T, std::string>, "cannot serialize vector of strings as tuple");
		serialize_type<std::vector<T>>();
		serialize_length(data.size());

		*this << '(';
		for (const auto& s : data)
			serialize(s);
		return *this << ')';
	}

	template<typename T>
	constexpr Response& serialize_type() {
		return *this << type_to_string<T>::value;
	}

	// serialize a string's body to a Response
	Response& serialize_string_body(const std::string &data);

	// serialize a length to a Response
	Response& serialize_length(size_t length);
};

} // namespace vanity

#endif //VANITY_RESPONSE_H
