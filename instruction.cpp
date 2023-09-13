//
// Created by kingsli on 9/13/23.
//

#include "instruction.h"

namespace vanity {

static inline void skip_whitespace(const std::string& msg, size_t& pos)
{
	while (pos < msg.size() and isspace(msg[pos]))
		++pos;
}

std::string extract_word(const std::string& msg, size_t& pos)
{
	skip_whitespace(msg, pos);
	if (pos >= msg.size()) {
		throw InvalidInstruction("unexpected end of message");
	}

	if (msg[pos] != '"') {
		throw InvalidInstruction("word not opened with quotes");
	}
	++pos;

	std::string word;
	while (pos < msg.size()) {
		if (msg[pos] == '"') {
			++pos;
			return word;
		}
		if (msg[pos] == '\\' and pos + 1 < msg.size() and msg[pos + 1] == '"') {
			++pos;
		}
		word += msg[pos];
		++pos;
	}

	throw InvalidInstruction("word not closed with quotes");
}

std::string extract_single_word(const std::string& msg, size_t& pos)
{
	std::string word = extract_word(msg, pos);

	skip_whitespace(msg, pos);
	if (pos < msg.size()) {
		throw InvalidInstruction("unexpected character at end of message");
	}

	return word;
}

std::pair<std::string, std::string> extract_key_value(const std::string& msg, size_t& pos)
{
	std::string key = extract_word(msg, pos);
	std::string value = extract_word(msg, pos);

	skip_whitespace(msg, pos);
	if (pos < msg.size()) {
		throw InvalidInstruction("unexpected character at end of message");
	}

	return std::make_pair(key, value);
}

unique_void_ptr parse_get(const std::string& msg, size_t& pos)
{
	auto [key, value] = extract_key_value(msg, pos);
	return make_unique_void<get_data_t>(key, value);
}

unique_void_ptr parse_set(const std::string& msg, size_t& pos)
{
	auto [key, value] = extract_key_value(msg, pos);
	return make_unique_void<set_data_t>(key, value);
}

unique_void_ptr parse_del(const std::string& msg, size_t& pos)
{
	return make_unique_void<del_data_t>(extract_single_word(msg, pos));
}

instruction_t parse_instruction(const std::string& msg)
{
	size_t pos = 0;
	skip_whitespace(msg, pos);
	if (pos >= msg.size()) {
		throw InvalidInstruction("message is blank");
	}

	if (pos + 3 < msg.size()) {
		if (msg[pos] == 'G' and msg[pos + 1] == 'E' and msg[pos + 2] == 'T') {
			pos += 3;
			return {operation_t::GET, parse_get(msg, pos)};
		}
		if (msg[pos] == 'S' and msg[pos + 1] == 'E' and msg[pos + 2] == 'T') {
			pos += 3;
			return {operation_t::SET, parse_set(msg, pos)};
		}
		if (msg[pos] == 'D' and msg[pos + 1] == 'E' and msg[pos + 2] == 'L') {
			pos += 3;
			return {operation_t::DEL, parse_del(msg, pos)};
		}
		throw InvalidInstruction("invalid operation");
	}
	throw InvalidInstruction("instruction too short");
}

} // namespace vanity