//
// Created by kingsli on 5/3/24.
//

#include <stdexcept>

#include "reply_message.h"


namespace vanity {

const std::array<const char*, ReplyMessage::M_STATUS_COUNT> ReplyMessage::M_STATUS_STRINGS = ReplyMessage::init_status_strings();

auto ReplyMessage::init_status_strings() -> std::array<const char*, M_STATUS_COUNT> {
	std::array<const char*, M_STATUS_COUNT> strings {};

	for (auto& [op, str] : REPLY_STATUS_STRINGS)
		strings[static_cast<uint>(op)] = str.c_str();

	return strings;
}

ReplyMessage::ReplyMessage() : Sendable() {
	*this << "REPLY";
}

ReplyMessage &ReplyMessage::operator<<(const std::string &data) {
	this->Sendable::operator<<(data);
	return *this;
}

ReplyMessage &ReplyMessage::operator<<(const char *data) {
	this->Sendable::operator<<(data);
	return *this;
}

ReplyMessage &ReplyMessage::operator<<(const char data) {
	this->Sendable::operator<<(data);
	return *this;
}

ReplyMessage &ReplyMessage::operator<<(ReplyStatus status) {
	if (static_cast<uint>(status) >= M_STATUS_COUNT)
		throw std::invalid_argument("invalid status");

	return *this << M_STATUS_STRINGS[static_cast<uint>(status)];
}

ReplyMessage &ReplyMessage::serialize(const std::string &data) {
	reserve(data.size() + 6);
	serialize_length(data.size());
	return *this << data;
}

ReplyMessage &ReplyMessage::serialize(int64_t data) {
	return *this << std::to_string(data) << ' ';
}

ReplyMessage &ReplyMessage::serialize_length(size_t length) {
	return *this << '(' << std::to_string(length) << ')';
}

ReplyMessage &ReplyMessage::serialize(const std::unordered_set<std::string> &data) {
	serialize_length(data.size());

	*this << '{';
	for (const auto &s: data)
		serialize(s);
	return *this << '}';
}

ReplyMessage &ReplyMessage::serialize(bool data) {
	return *this << (data ? "true" : "false");
}

ReplyMessage &ReplyMessage::serialize(const char *data) {
	return serialize(std::string(data));
}

} // namespace vanity