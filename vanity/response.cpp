//
// Created by kingsli on 10/26/23.
//

#include <cstring>
#include <stdexcept>

#include "response.h"


namespace vanity {

Response::Response() {
	m_data.resize(M_LENGTH_SIZE);
}

std::string&& Response::extract_data() {
	auto size = htonl(m_data.size() - M_LENGTH_SIZE);
	std::memcpy(m_data.data(), &size, M_LENGTH_SIZE);
	return std::move(m_data);
}

Response& Response::add_ok(const std::string &data) {
	return add(ok, data);
}

Response& Response::add_ok() {
	return add(ok);
}

Response& Response::add_error(const std::string &data) {
	return add(error, data);
}

Response& Response::add_error() {
	return add(error);
}

Response& Response::add_null(const std::string &data) {
	return add(null, data);
}

Response& Response::add_null() {
	return add(null);
}

Response& Response::add_pong(const std::string &data) {
	return add(pong, data);
}

Response& Response::add_pong() {
	return add(pong);
}

Response& Response::add_denied(const std::string &data) {
	return add(denied, data);
}

Response& Response::add_denied() {
	return add(denied);
}

Response& Response::add_internal_error(const std::string &data) {
	return add(internal_error, data);
}

Response& Response::add_internal_error() {
	return add(internal_error);
}

Response& Response::add_bad_type(const std::string &data) {
	return add(bad_type, data);
}

Response& Response::add_bad_type() {
	return add(bad_type);
}

Response& Response::add_bad_request(const std::string &data) {
	return add(bad_request, data);
}

Response& Response::add_bad_request() {
	return add(bad_request);
}

Response& Response::add(Status status, const std::string &data) {
	return add(status) << data;
}

Response& Response::add(Status status) {
	switch (status) {
		case ok:
			return *this << status_value::ok;
		case error:
			return *this << status_value::error;
		case null:
			return *this << status_value::null;
		case pong:
			return *this << status_value::pong;
		case denied:
			return *this << status_value::denied;
		case internal_error:
			return *this << status_value::internal_error;
		case bad_type:
			return *this << status_value::bad_type;
		case bad_request:
			return *this << status_value::bad_request;
		default:
			throw std::runtime_error("invalid status");
	}
}

Response &Response::operator<<(const std::string &data) {
	m_data += data;
	return *this;
}

Response &Response::operator<<(const char *data) {
	m_data += data;
	return *this;
}

std::string serialize(const std::string &data) {
	std::string ret {};
	ret.reserve(data.size() + 10);
	ret += serialize_type<std::string>();

	ret += "\"";
	for (char c : data) {
		if (c == '"')
			ret += "\\\"";
		else
			ret += c;
	}
	ret += "\"";

	return ret;
}

std::string serialize(int64_t data) {
	return serialize_type<int64_t>() + std::to_string(data);
}

std::string serialize(double data) {
	return serialize_type<double>() + std::to_string(data);
}

} // namespace vanity