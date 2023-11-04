//
// Created by kingsli on 10/26/23.
//

#ifndef VANITY_RESPONSE_H
#define VANITY_RESPONSE_H

#include <list>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

#include "db/base_database.h"


namespace vanity {

using primary_serialize_type = db::BaseDatabase::data_type;

// convert a type to a string
template <typename T>
struct type_to_string;

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

template <typename T>
struct type_to_string<std::vector<T>> {
	static constexpr const char* value = ":ARR ";
};

template <>
struct type_to_string<std::list<std::string>> {
	static constexpr const char* value = ":LIST ";
};

template <>
struct type_to_string<std::unordered_set<std::string>> {
	static constexpr const char* value = ":SET ";
};

template <>
struct type_to_string<bool> {
	static constexpr const char* value = ":BOOL ";
};

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
	};

	// default constructor
	Response();

	// destructively extract the response data
	std::string&& extract_data();

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

	// serialize the database's primary type to a Response
	Response& serialize(const primary_serialize_type& data);

	// serialize nothing to a Response (does nothing)
	Response& serialize();

	// do not serialize const char*
	Response& serialize(const char *data) = delete;

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
		serialize_type<std::vector<T>>()
			  << '(' + std::to_string(data.size()) + ")";

		*this << '[';
		for (const auto& s : data)
			serialize(s);
		return *this << ']';
	}

	template<typename T>
	constexpr Response& serialize_type() {
		return *this << type_to_string<T>::value;
	}

private:
	// serialize a string's body to a Response
	Response& serialize_body(const std::string &data);

	struct status_value {
		static constexpr char const* const ok = "OK";
		static constexpr char const* const error = "ERROR";
		static constexpr char const* const null = "NULL";
		static constexpr char const* const pong = "PONG";
		static constexpr char const* const denied = "DENIED";
		static constexpr char const* const internal_error = "INTERNAL_ERROR";
		static constexpr char const* const bad_type = "BAD_TYPE";
		static constexpr char const* const bad_request = "BAD_REQUEST";
	};
};

// make an OK response
template<typename... Args>
Response ok(Args&&... args){
	return (Response() << Response::Status::ok).serialize(args...);
}

// a plain OK response
inline Response ok(const char* body) {
	return Response() << Response::Status::ok << body;
}

// make an ERROR response
template<typename... Args>
Response error(Args&&... args) {
	return (Response() << Response::Status::error).serialize(args...);
}

// a plain ERROR response
inline Response error(const char* body) {
	return Response() << Response::Status::error << body;
}

// make a NULL response
template<typename... Args>
Response null(Args&&... args) {
	return (Response() << Response::Status::null).serialize(args...);
}

// a plain NULL response
inline Response null(const char* body) {
	return Response() << Response::Status::null << body;
}

// make a PONG response
template<typename... Args>
Response pong(Args&&... args) {
	return (Response() << Response::Status::pong).serialize(args...);
}

// a plain PONG response
inline Response pong(const char* body) {
	return Response() << Response::Status::pong << body;
}

// make a DENIED response
template<typename... Args>
Response denied(Args&&... args) {
	return (Response() << Response::Status::denied).serialize(args...);
}

// a plain DENIED response
inline Response denied(const char* body) {
	return Response() << Response::Status::denied << body;
}

// make an INTERNAL_ERROR response
template<typename... Args>
Response internal_error(Args&&... args) {
	return (Response() << Response::Status::internal_error).serialize(args...);
}

// a plain INTERNAL_ERROR response
inline Response internal_error(const char* body) {
	return Response() << Response::Status::internal_error << body;
}

// make a BAD_TYPE response
template<typename... Args>
Response bad_type(Args&&... args) {
	return (Response() << Response::Status::bad_type).serialize(args...);
}

// a plain BAD_TYPE response
inline Response bad_type(const char* body) {
	return Response() << Response::Status::bad_type << body;
}

// make a BAD_REQUEST response
template<typename... Args>
Response bad_request(Args&&... args) {
	return (Response() << Response::Status::bad_request).serialize(args...);
}

// a plain BAD_REQUEST response
inline Response bad_request(const char* body) {
	return Response() << Response::Status::bad_request << body;
}

} // namespace vanity

#endif //VANITY_RESPONSE_H
