//
// Created by kingsli on 5/3/24.
//

#include "reply_message.h"

namespace vanity {

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

} // namespace vanity