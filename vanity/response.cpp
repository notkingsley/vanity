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

inline void serialize_body(const std::string &data, std::string& str) {
	str.reserve(str.size() + data.size() + 10);
	str += '(' + std::to_string(data.size()) + ")";
	str += data;
}

void serialize(const std::string &data, std::string& str) {
	str.reserve(str.size() + data.size() + 10);
	str += serialize_type<std::string>();
	serialize_body(data, str);
}

void serialize(int64_t data, std::string& str) {
	str += serialize_type<int64_t>();
	str += std::to_string(data);
}

void serialize(size_t data, std::string& str) {
	serialize(int64_t(data), str);
}

void serialize(double data, std::string& str) {
	str += serialize_type<double>();
	str += std::to_string(data);
}

void serialize(const std::variant<std::string, int64_t , double, std::list<std::string>>& data, std::string& str) {
	switch (data.index()) {
		case 0:
		{
			serialize(std::get<0>(data), str);
			break;
		}
		case 1:
		{
			serialize(std::get<1>(data), str);
			break;
		}
		case 2:
		{
			serialize(std::get<2>(data), str);
			break;
		}
		case 3:
		{
			serialize(std::get<3>(data), str);
			break;
		}
		default:
			throw std::runtime_error("invalid type");
	}
}

void serialize(const std::list<std::string>& data, std::string& str) {
	str += serialize_type<std::list<std::string>>();
	str += '(' + std::to_string(data.size()) + ")";

	str += '[';
	for (const auto& s : data)
		serialize_body(s, str);
	str += ']';
}

} // namespace vanity