//
// Created by kingsli on 10/26/23.
//

#include <stdexcept>

#include "response.h"


namespace vanity {

Response &&Response::move() {
	return std::move(*this);
}

Response &Response::operator<<(Response::Status status) {
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
		case bad_state:
			return *this << status_value::bad_state;
		case queued:
			return *this << status_value::queued;
		case async:
			return *this << status_value::async;
		default:
			throw std::runtime_error("invalid status");
	}
}

Response &Response::operator<<(const std::string &data) {
	this->Sendable::operator<<(data);
	return *this;
}

Response &Response::operator<<(const char *data) {
	this->Sendable::operator<<(data);
	return *this;
}

Response &Response::operator<<(const char data) {
	this->Sendable::operator<<(data);
	return *this;
}

Response &Response::serialize(bool data) {
	return serialize_type<bool>() << (data ? "true" : "false");
}

Response &Response::serialize_string_body(const std::string &data) {
	reserve(data.size() + 10);
	serialize_length(data.size());
	return *this << data;
}

Response &Response::serialize(const std::string &data) {
	serialize_type<std::string>();
	return serialize_string_body(data);
}

Response &Response::serialize(int64_t data) {
	return serialize_type<int64_t>() << std::to_string(data);
}

Response &Response::serialize(size_t data) {
	return serialize(int64_t(data));
}

Response &Response::serialize(double data) {
	return serialize_type<double>() << std::to_string(data);
}

Response &Response::serialize(const std::list<std::string> &data) {
	serialize_type<std::list<std::string>>();
	serialize_length(data.size());

	*this << '[';
	for (const auto& s : data)
		serialize_string_body(s);
	return *this << ']';
}

Response &Response::serialize(const std::unordered_set<std::string> &data) {
	serialize_type<std::unordered_set<std::string>>();
	serialize_length(data.size());

	*this << '{';
	for (const auto &s: data)
		serialize_string_body(s);
	return *this << '}';
}

Response &Response::serialize(const std::unordered_map<std::string, std::string> &data) {
	serialize_type<std::unordered_map<std::string, std::string>>();
	serialize_length(data.size());

	*this << '{';
	for (const auto &s: data) {
		serialize_string_body(s.first);
		serialize_string_body(s.second);
	}
	return *this << '}';
}

Response &Response::serialize(const primary_serialize_type &data) {
	switch (data.index()) {
		case 0:
		{
			return serialize(std::get<0>(data));
		}
		case 1:
		{
			return serialize(std::get<1>(data));
		}
		case 2:
		{
			return serialize(std::get<2>(data));
		}
		case 3:
		{
			return serialize(std::get<3>(data));
		}
		case 4:
		{
			return serialize(std::get<4>(data));
		}
		case 5:
		{
			return serialize(std::get<5>(data));
		}
		default:
			throw std::runtime_error("invalid type");
	}
}

Response &Response::serialize(client_auth data) {
	serialize_type<client_auth>();
	switch (data) {
		case client_auth::USER:
			return *this << "USER";
		case client_auth::ADMIN:
			return *this << "ADMIN";
		case client_auth::UNKNOWN:
			return *this << "UNKNOWN";
		case client_auth::PEER:
			return *this << "PEER";
		default:
			throw std::runtime_error("invalid type");
	}
}

Response &Response::serialize() {
	return *this;
}

Response &Response::serialize_length(size_t length) {
	return *this << '(' << std::to_string(length) << ')';
}

} // namespace vanity