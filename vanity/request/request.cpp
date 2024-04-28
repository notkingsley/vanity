//
// Created by kingsli on 12/21/23.
//

#include "exceptions.h"
#include "request.h"


namespace vanity {

Request::Request(const std::string &msg): Extractable(msg) {}

std::string Request::format() const {
	return m_msg + " at " + std::to_string(m_pos);
}

size_t Request::index() const {
	return m_pos;
}

std::string_view Request::view(size_t start, size_t end) const {
	if (start > end)
		throw std::out_of_range("start index greater than end index");

	if (end > m_msg.size())
		throw std::out_of_range("end index out of range");

	return {m_msg.data() + start, end - start};
}

operation_t Request::get_operation() {
	return get_from_list(OPERATION_T_STRINGS, "invalid operation");
}

operation_t Request::peek_operation() {
	return peek_from_list(OPERATION_T_STRINGS, "invalid operation");
}

object_t Request::get_object_t() {
	return get_from_list(OBJECT_T_STRINGS, "invalid object type");
}

} // namespace vanity
