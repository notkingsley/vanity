//
// Created by kingsli on 10/26/23.
//

#ifndef VANITY_RESPONSE_H
#define VANITY_RESPONSE_H

#include <concepts>
#include <netinet/in.h>

#include "db/db/types.h"


namespace vanity {

using primary_serialize_type = db::db_data_type;

// convert a type to a string
template <typename T>
struct type_to_string;

template <>
struct type_to_string<bool> {
	static constexpr const char* value = ":BOOL ";
};

template <>
struct type_to_string<int64_t> {
	static constexpr const char* value = ":INT ";
};

template <>
struct type_to_string<double> {
	static constexpr const char* value = ":FLOAT ";
};

template <>
struct type_to_string<std::string> {
	static constexpr const char* value = ":STR ";
};

template <>
struct type_to_string<std::monostate> {
	static constexpr const char* value = ":NULL ";
};

template <>
struct type_to_string<std::vector<std::string>> {
	static constexpr const char* value = ":ARR ";
};

template <typename T>
struct type_to_string<std::vector<T>> {
	static constexpr const char* value = ":TUPLE ";
};

template <>
struct type_to_string<db::list_t> {
	static constexpr const char* value = ":LIST ";
};

template <>
struct type_to_string<db::set_t> {
	static constexpr const char* value = ":SET ";
};

template <>
struct type_to_string<db::hash_t> {
	static constexpr const char* value = ":HASH ";
};

// type-dependent std::false_type to delay evaluation till instantiation
template <typename T>
struct false_type : std::false_type {};

/*
 * A Response allows a dynamically efficient method to compose a response
 */
class Response
{
private:
	// the size of the length field
	static constexpr auto M_LENGTH_SIZE = sizeof(decltype(htonl(0)));

	// the response data
	std::string m_data;

	struct status_value {
		static constexpr char const* const ok = "OK";
		static constexpr char const* const error = "ERROR";
		static constexpr char const* const null = "NULL";
		static constexpr char const* const pong = "PONG";
		static constexpr char const* const denied = "DENIED";
		static constexpr char const* const internal_error = "INTERNAL_ERROR";
		static constexpr char const* const bad_type = "BAD_TYPE";
		static constexpr char const* const bad_request = "BAD_REQUEST";
		static constexpr char const* const bad_state = "BAD_STATE";
		static constexpr char const* const queued = "QUEUED";
		static constexpr char const* const async = "ASYNC";
	};

public:
	enum Status{
		ok,
		error,
		null,
		pong,
		denied,
		internal_error,
		bad_type,
		bad_request,
		bad_state,
		queued,
		async,
	};

	// default constructor
	Response();

	// destructively extract the response data
	std::string&& extract_data();

	// obtain a view of the (current) response body (without the length field)
	std::string_view body() const;

	// reserve space in the response
	void reserve(size_t size);

	// return this as a rvalue
	Response&& move();

	// add a message to the response
	Response& add(const std::string& data);

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

	// serialize nothing to a Response (does nothing)
	Response& serialize();

	// do not serialize const char* directly
	// call serialize_string_body to add a string,
	// or operator<< for raw data
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

// make a response with a value
template<typename T>
Response value_response(Response::Status status, T data) {
	return (Response() << status).serialize(data);
}

// make a response with a body
inline Response body_response(Response::Status status, const char* body) {
	return (Response() << status).serialize_string_body(body);
}

// make an empty response
inline Response empty_response(Response::Status status) {
	return Response() << status;
}

// make an OK response
template<typename T>
Response ok(T data){
	return value_response(Response::ok, data);
}

// an OK response with a message body
inline Response ok(const char* body) {
	return body_response(Response::ok, body);
}

// make an empty OK response
inline Response ok() {
	return empty_response(Response::ok);
}

// make an ERROR response
template<typename T>
Response error(T data) {
	return value_response(Response::error, data);
}

// an ERROR response with a message body
inline Response error(const char* body) {
	return body_response(Response::error, body);
}

// make an empty ERROR response
inline Response error() {
	return empty_response(Response::error);
}

// make a NULL response
template<typename T>
Response null(T data) {
	return value_response(Response::null, data);
}

// a NULL response with a message body
inline Response null(const char* body) {
	return body_response(Response::null, body);
}

// make an empty NULL response
inline Response null() {
	return empty_response(Response::null);
}

// make an empty PONG response
inline Response pong() {
	return empty_response(Response::pong);
}

// make a DENIED response
template<typename T>
Response denied(T data) {
	return value_response(Response::denied, data);
}

// a DENIED response with a message body
inline Response denied(const char* body) {
	return body_response(Response::denied, body);
}

// make an empty DENIED response
inline Response denied() {
	return empty_response(Response::denied);
}

// make an INTERNAL_ERROR response
template<typename T>
Response internal_error(T data) {
	return value_response(Response::internal_error, data);
}

// an INTERNAL_ERROR response with a message body
inline Response internal_error(const char* body) {
	return body_response(Response::internal_error, body);
}

// make an empty INTERNAL_ERROR response
inline Response internal_error() {
	return empty_response(Response::internal_error);
}

// make a BAD_TYPE response
template<typename T>
Response bad_type(T data) {
	return value_response(Response::bad_type, data);
}

// a BAD_TYPE response with a message body
inline Response bad_type(const char* body) {
	return body_response(Response::bad_type, body);
}

// make an empty BAD_TYPE response
inline Response bad_type() {
	return empty_response(Response::bad_type);
}

// make a BAD_REQUEST response
template<typename T>
Response bad_request(T data) {
	return value_response(Response::bad_request, data);
}

// a BAD_REQUEST response with a message body
inline Response bad_request(const char* body) {
	return body_response(Response::bad_request, body);
}

// make an empty BAD_REQUEST response
inline Response bad_request() {
	return empty_response(Response::bad_request);
}

// make a BAD_STATE response
template<typename T>
Response bad_state(T data) {
	return value_response(Response::bad_state, data);
}

// a BAD_STATE response with a message body
inline Response bad_state(const char* body) {
	return body_response(Response::bad_state, body);
}

// make an empty BAD_STATE response
inline Response bad_state() {
	return empty_response(Response::bad_state);
}

// make a QUEUED response
template<typename T>
Response queued(T data) {
	return value_response(Response::queued, data);
}

// a QUEUED response with a message body
inline Response queued(const char* body) {
	return body_response(Response::queued, body);
}

// make an empty QUEUED response
inline Response queued() {
	return empty_response(Response::queued);
}

// make an ASYNC response
template<typename T>
Response async(T data) {
	return value_response(Response::async, data);
}

// an ASYNC response with a message body
inline Response async(const char* body) {
	return body_response(Response::async, body);
}

// make an empty ASYNC response
inline Response async() {
	return empty_response(Response::async);
}

} // namespace vanity

#endif //VANITY_RESPONSE_H
