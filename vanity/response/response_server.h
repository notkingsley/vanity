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
		return empty_response(status).serialize(data);
	}

	// make an error response
	static Response error_response(Response::Status status, const char* body) {
		return empty_response(status).serialize(error_t{body});
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

	// make an empty OK response
	static Response ok() {
		return empty_response(Response::ok);
	}

	// make an empty NULL response
	static Response null() {
		return empty_response(Response::null);
	}

	// make an empty PONG response
	static Response pong() {
		return empty_response(Response::pong);
	}

	// make an empty DENIED response
	static Response denied() {
		return empty_response(Response::denied);
	}

	// make an empty QUEUED response
	static Response queued() {
		return empty_response(Response::queued);
	}

	// make an empty BAD_STATE response
	static Response bad_state() {
		return empty_response(Response::bad_state);
	}

	// an ERROR response
	static Response error(const char* body) {
		return error_response(Response::error, body);
	}

	// a BAD_TYPE response
	static Response bad_type(const char* body) {
		return error_response(Response::bad_type, body);
	}

	// a BAD_REQUEST response
	static Response bad_request(const char* body) {
		return error_response(Response::bad_request, body);
	}

	// an INTERNAL_ERROR response
	static Response internal_error(const char* body) {
		return error_response(Response::internal_error, body);
	}

	// make an ASYNC response
	template<typename T>
	static Response async(const T& data) {
		return value_response(Response::async, data);
	}
};

} // namespace vanity

#endif //VANITY_RESPONSE_SERVER_H
