//
// Created by kingsli on 4/29/24.
//

#include "post_message.h"

namespace vanity {

const std::array<const char*, PostMessage::M_OP_COUNT> PostMessage::M_OP_STRINGS = PostMessage::init_op_strings();

auto PostMessage::init_op_strings() -> std::array<const char*, M_OP_COUNT> {
	std::array<const char*, M_OP_COUNT> strings {};

	for (auto& [op, str] : PEER_OP_STRINGS)
		strings[static_cast<uint>(op)] = str.c_str();

	return strings;
}

PostMessage::PostMessage() : Sendable() {
	*this << "POST";
}

PostMessage::PostMessage(PostMessage::as_plain_user_t) { }

PostMessage& PostMessage::operator<<(peer_op_t op) {
	*this << M_OP_STRINGS[static_cast<uint>(op)];
	return *this;
}

PostMessage &PostMessage::operator<<(const std::string &data) {
	this->Sendable::operator<<(data);
	return *this;
}

PostMessage &PostMessage::operator<<(const char *data) {
	this->Sendable::operator<<(data);
	return *this;
}

PostMessage &PostMessage::operator<<(const char data) {
	this->Sendable::operator<<(data);
	return *this;
}

PostMessage &PostMessage::serialize(const std::string &data) {
	reserve(data.size() + 6);
	serialize_length(data.size());
	return *this << data;
}

PostMessage &PostMessage::serialize(int64_t data) {
	return *this << std::to_string(data) << ' ';
}

PostMessage &PostMessage::serialize_length(size_t length) {
	return *this << '(' << std::to_string(length) << ')';
}

} // namespace vanity
