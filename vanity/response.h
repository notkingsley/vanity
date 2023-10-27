//
// Created by kingsli on 10/26/23.
//

#ifndef VANITY_RESPONSE_H
#define VANITY_RESPONSE_H

#include <array>
#include <netinet/in.h>
#include <string>
#include <queue>

namespace vanity {

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

	// add a message to the response
	Response& add(Status status, const std::string& data);

	// add a message to the response
	Response& add(Status status);

	// add an OK message to the response
	Response& add_ok(const std::string& data);

	// add an OK message to the response
	Response& add_ok();

	// add an ERROR message to the response
	Response& add_error(const std::string& data);

	// add an ERROR message to the response
	Response& add_error();

	// add a NULL message to the response
	Response& add_null(const std::string& data);

	// add a NULL message to the response
	Response& add_null();

	// add a PONG message to the response
	Response& add_pong(const std::string& data);

	// add a PONG message to the response
	Response& add_pong();

	// add a DENIED message to the response
	Response& add_denied(const std::string& data);

	// add a DENIED message to the response
	Response& add_denied();

	// add an INTERNAL_ERROR message to the response
	Response& add_internal_error(const std::string& data);

	// add an INTERNAL_ERROR message to the response
	Response& add_internal_error();

	// add a BAD_TYPE message to the response
	Response& add_bad_type(const std::string& data);

	// add a BAD_TYPE message to the response
	Response& add_bad_type();

	// add a BAD_REQUEST message to the response
	Response& add_bad_request(const std::string& data);

	// add a BAD_REQUEST message to the response
	Response& add_bad_request();

private:
	// add a message to the response
	Response& operator<<(const std::string& data);

	// add a message to the response
	Response& operator<<(const char* data);

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

// serialize a type to a string
template<typename T>
constexpr auto serialize_type()
{
	return type_to_string<T>::value;
}

// serialize a string object to a string
std::string serialize(const std::string& data);

// serialize an int object to a string
std::string serialize(int64_t data);

// serialize a double object to a string
std::string serialize(double data);

// make an OK response
template<typename... Args>
Response ok(Args&&... args){
	return Response().add_ok(args...);
}

// make an ERROR response
template<typename... Args>
Response error(Args&&... args) {
	return Response().add_error(args...);
}

// make a NULL response
template<typename... Args>
Response null(Args&&... args) {
	return Response().add_null(args...);
}

// make a PONG response
template<typename... Args>
Response pong(Args&&... args) {
	return Response().add_pong(args...);
}

// make a DENIED response
template<typename... Args>
Response denied(Args&&... args) {
	return Response().add_denied(args...);
}

// make an INTERNAL_ERROR response
template<typename... Args>
Response internal_error(Args&&... args) {
	return Response().add_internal_error(args...);
}

// make a BAD_TYPE response
template<typename... Args>
Response bad_type(Args&&... args) {
	return Response().add_bad_type(args...);
}

// make a BAD_REQUEST response
template<typename... Args>
Response bad_request(Args&&... args) {
	return Response().add_bad_request(args...);
}

} // namespace vanity

#endif //VANITY_RESPONSE_H
