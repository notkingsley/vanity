//
// Created by kingsli on 9/14/23.
//

#include "instruction_server.h"


namespace vanity {

// all the possible operations
enum class operation_t{
	GET,
	SET,
	DEL,
	EXIT,
	TERMINATE,
};

// increment pos until it is not a whitespace
static inline void skip_whitespace(const std::string& msg, size_t& pos)
{
	while (pos < msg.size() and isspace(msg[pos]))
		++pos;
}

// extract the operation from the message
static inline operation_t extract_operation(const std::string& msg, size_t& pos)
{
	skip_whitespace(msg, pos);
	if (msg.compare(pos, 3, "GET") == 0) {
		pos += 3;
		return operation_t::GET;
	}
	if (msg.compare(pos, 3, "SET") == 0) {
		pos += 3;
		return operation_t::SET;
	}
	if (msg.compare(pos, 3, "DEL") == 0) {
		pos += 3;
		return operation_t::DEL;
	}
	if (msg.compare(pos, 4, "EXIT") == 0) {
		pos += 4;
		return operation_t::EXIT;
	}
	if (msg.compare(pos, 9, "TERMINATE") == 0){
		pos += 9;
		return operation_t::TERMINATE;
	}
	throw InvalidInstruction("invalid operation");
}

// extract a word from part of a message
static inline std::string extract_word(const std::string& msg, size_t& pos)
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

// ensure we are at the end of the message
static inline void ensure_end(const std::string& msg, size_t& pos)
{
	skip_whitespace(msg, pos);
	if (pos < msg.size()) {
		throw InvalidInstruction("unexpected character at end of message");
	}
}

// extract exactly a single word from the rest of a message
// throw if there is more than one word
static inline std::string extract_single_word(const std::string& msg, size_t& pos)
{
	std::string word = extract_word(msg, pos);
	ensure_end(msg, pos);
	return word;
}

// extract exactly a key-value pair from the rest of a message
// throw if there is more than one pair
static inline std::pair<std::string, std::string> extract_key_value(const std::string& msg, size_t& pos)
{
	std::string key = extract_word(msg, pos);
	std::string value = extract_word(msg, pos);
	ensure_end(msg, pos);
	return std::make_pair(key, value);
}

void InstructionServer::handle(const std::string& msg, const ClientSocket& socket) {
	try{
		size_t pos = 0;
		operation_t op = extract_operation(msg, pos);

		switch (op) {
			case operation_t::GET:
			{
				instruction_get(socket, extract_single_word(msg, pos));
				break;
			}
			case operation_t::SET:
			{
				auto [key, value] = extract_key_value(msg, pos);
				instruction_set(socket, key, value);
				break;
			}
			case operation_t::DEL:
			{
				instruction_del(socket, extract_single_word(msg, pos));
				break;
			}
			case operation_t::EXIT:
			{
				ensure_end(msg, pos);
				instruction_exit(socket);
				break;
			}
			case operation_t::TERMINATE:
			{
				ensure_end(msg, pos);
				instruction_terminate(socket);
				break;
			}
		}
	}
	catch (const InvalidInstruction& e) {
		send(socket, server_constants::error + make_message(e.what()));
	}
}

std::string InstructionServer::make_message(const std::string &msg) {
	std::string ret {};
	ret.reserve(msg.size() + 2);

	ret += "\"";
	for (char c : msg) {
		if (c == '"')
			ret += "\\\"";
		else
			ret += c;
	}
	ret += "\"";

	return ret;
}

} // namespace vanity
