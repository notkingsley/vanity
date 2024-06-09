//
// Created by kingsli on 6/10/24.
//

#include "async_message.h"

namespace vanity {

const std::array<const char*, AsyncMessage::M_OP_COUNT> AsyncMessage::M_ASYNC_OP_STRINGS = AsyncMessage::init_op_strings();

auto AsyncMessage::init_op_strings() -> std::array<const char*, M_OP_COUNT> {
	std::array<const char*, M_OP_COUNT> strings {};

	for (auto& [op, str] : ASYNC_OP_STRINGS)
		strings[static_cast<uint>(op)] = str.c_str();

	return strings;
}

AsyncMessage::AsyncMessage() : Sendable() {
	*this << "ASYNC";
}

AsyncMessage& AsyncMessage::operator<<(async_op_t op) {
	*this << M_ASYNC_OP_STRINGS[static_cast<uint>(op)];
	return *this;
}

AsyncMessage &AsyncMessage::operator<<(const std::string &data) {
	this->Sendable::operator<<(data);
	return *this;
}

AsyncMessage &AsyncMessage::operator<<(const char *data) {
	this->Sendable::operator<<(data);
	return *this;
}

AsyncMessage &AsyncMessage::operator<<(const char data) {
	this->Sendable::operator<<(data);
	return *this;
}

AsyncMessage &AsyncMessage::serialize(const std::string &data) {
	reserve(data.size() + 6);
	serialize_length(data.size());
	return *this << data;
}

AsyncMessage &AsyncMessage::serialize(int64_t data) {
	return *this << std::to_string(data) << ' ';
}

AsyncMessage &AsyncMessage::serialize_length(size_t length) {
	return *this << '(' << std::to_string(length) << ')';
}

} // namespace vanity
