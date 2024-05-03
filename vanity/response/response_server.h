//
// Created by kingsli on 1/2/24.
//

#ifndef VANITY_RESPONSE_SERVER_H
#define VANITY_RESPONSE_SERVER_H

#include "response.h"

namespace vanity {

/*
 * A ResponseServer simplifies creating responses
 * by providing handy shortcuts for common responses
 */
class ResponseServer
{
private:
	// make a response with a value
	template<typename T>
	static Response value_response(Response::Status status, const T& data) {
		return (Response() << status).serialize(data);
	}

	// make a response with a body
	static Response body_response(Response::Status status, const char* body) {
		return (Response() << status).serialize_string_body(body);
	}

	// make an empty response
	static Response empty_response(Response::Status status) {
		return Response() << status;
	}

protected:
	// make an OK response
	template<typename T>
	static Response ok(const T& data){
		return value_response(Response::ok, data);
	}

	// an OK response with a message body
	static Response ok(const char* body) {
		return body_response(Response::ok, body);
	}

	// make an empty OK response
	static Response ok() {
		return empty_response(Response::ok);
	}

	// make an ERROR response
	template<typename T>
	static Response error(const T& data) {
		return value_response(Response::error, data);
	}

	// an ERROR response with a message body
	static Response error(const char* body) {
		return body_response(Response::error, body);
	}

	// make an empty ERROR response
	static Response error() {
		return empty_response(Response::error);
	}

	// make a NULL response
	template<typename T>
	static Response null(const T& data) {
		return value_response(Response::null, data);
	}

	// a NULL response with a message body
	static Response null(const char* body) {
		return body_response(Response::null, body);
	}

	// make an empty NULL response
	static Response null() {
		return empty_response(Response::null);
	}

	// make an empty PONG response
	static Response pong() {
		return empty_response(Response::pong);
	}

	// make a DENIED response
	template<typename T>
	static Response denied(const T& data) {
		return value_response(Response::denied, data);
	}

	// a DENIED response with a message body
	static Response denied(const char* body) {
		return body_response(Response::denied, body);
	}

	// make an empty DENIED response
	static Response denied() {
		return empty_response(Response::denied);
	}

	// make an INTERNAL_ERROR response
	template<typename T>
	static Response internal_error(const T& data) {
		return value_response(Response::internal_error, data);
	}

	// an INTERNAL_ERROR response with a message body
	static Response internal_error(const char* body) {
		return body_response(Response::internal_error, body);
	}

	// make an empty INTERNAL_ERROR response
	static Response internal_error() {
		return empty_response(Response::internal_error);
	}

	// make a BAD_TYPE response
	template<typename T>
	static Response bad_type(const T& data) {
		return value_response(Response::bad_type, data);
	}

	// a BAD_TYPE response with a message body
	static Response bad_type(const char* body) {
		return body_response(Response::bad_type, body);
	}

	// make an empty BAD_TYPE response
	static Response bad_type() {
		return empty_response(Response::bad_type);
	}

	// make a BAD_REQUEST response
	template<typename T>
	static Response bad_request(const T& data) {
		return value_response(Response::bad_request, data);
	}

	// a BAD_REQUEST response with a message body
	static Response bad_request(const char* body) {
		return body_response(Response::bad_request, body);
	}

	// make an empty BAD_REQUEST response
	static Response bad_request() {
		return empty_response(Response::bad_request);
	}

	// make a BAD_STATE response
	template<typename T>
	static Response bad_state(const T& data) {
		return value_response(Response::bad_state, data);
	}

	// a BAD_STATE response with a message body
	static Response bad_state(const char* body) {
		return body_response(Response::bad_state, body);
	}

	// make an empty BAD_STATE response
	static Response bad_state() {
		return empty_response(Response::bad_state);
	}

	// make a QUEUED response
	template<typename T>
	static Response queued(const T& data) {
		return value_response(Response::queued, data);
	}

	// a QUEUED response with a message body
	static Response queued(const char* body) {
		return body_response(Response::queued, body);
	}

	// make an empty QUEUED response
	static Response queued() {
		return empty_response(Response::queued);
	}

	// make an ASYNC response
	template<typename T>
	static Response async(const T& data) {
		return value_response(Response::async, data);
	}

	// an ASYNC response with a message body
	static Response async(const char* body) {
		return body_response(Response::async, body);
	}

	// make an empty ASYNC response
	static Response async() {
		return empty_response(Response::async);
	}
};

} // namespace vanity

#endif //VANITY_RESPONSE_SERVER_H
