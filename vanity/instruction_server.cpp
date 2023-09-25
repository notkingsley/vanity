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
	PERSIST,
	EXIT,
	TERMINATE,
	RESET,
	PING,
};

// all object types
enum class object_t{
	STR,
	INT,
	FLOAT,
};

// the concrete type of object_t
template <object_t>
struct concrete;

template <>
struct concrete<object_t::STR> {
	using type = std::string;
};

template <>
struct concrete<object_t::INT> {
	using type = int64_t;
};

template <>
struct concrete<object_t::FLOAT> {
	using type = double;
};

template<object_t _obj>
using concrete_t = typename concrete<_obj>::type;

// increment pos until it is not a whitespace
static inline void skip_whitespace(const std::string& msg, size_t& pos)
{
	while (pos < msg.size() and isspace(msg[pos]))
		++pos;
}

// ensure we are at the end of the message
static inline void ensure_end(const std::string& msg, size_t& pos)
{
	skip_whitespace(msg, pos);
	if (pos < msg.size())
		throw InvalidInstruction("unexpected character at end of message");
}

// ensure we are not at the end of the message
static inline void ensure_not_end(const std::string& msg, size_t& pos)
{
	skip_whitespace(msg, pos);
	if (pos >= msg.size())
		throw InvalidInstruction("unexpected end of message");
}

// extract the operation from the message
static inline operation_t extract_operation(const std::string& msg, size_t& pos)
{
	static std::initializer_list <std::pair<operation_t, std::string>> operations {
		{operation_t::GET,       "GET"},
		{operation_t::SET,       "SET"},
		{operation_t::DEL,       "DEL"},
		{operation_t::PERSIST,   "PERSIST"},
		{operation_t::EXIT,      "EXIT"},
		{operation_t::TERMINATE, "TERMINATE"},
		{operation_t::RESET,     "RESET"},
		{operation_t::PING,      "PING"},
	};
	skip_whitespace(msg, pos);
	for (const auto& [op, str] : operations) {
		if (msg.compare(pos, str.size(), str) == 0) {
			pos += str.size();
			return op;
		}
	}
	throw InvalidInstruction("invalid operation");
}

// extract the object type from the message
static inline object_t extract_object_t(const std::string& msg, size_t& pos)
{
	static char object_separator = ':';
	static std::initializer_list <std::pair<object_t, std::string>> objects {
		{object_t::STR,   "STR"},
		{object_t::INT,   "INT"},
		{object_t::FLOAT, "FLOAT"},
	};

	skip_whitespace(msg, pos);
	if (msg[pos] != object_separator) {
		throw InvalidInstruction("expected object type");
	}
	++pos;

	skip_whitespace(msg, pos);
	for (const auto& [obj, str] : objects) {
		if (msg.compare(pos, str.size(), str) == 0) {
			pos += str.size();
			return obj;
		}
	}
	throw InvalidInstruction("invalid object type");
}

// extract an object from part of a message
template<object_t obj>
inline concrete_t<obj> extract(const std::string& msg, size_t& pos);

// extract an int64_t from part of a message
template<>
inline int64_t extract<object_t::INT>(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
	size_t count = 0;
	auto ret{std::stoll(msg.substr(pos), &count)};
	pos += count;
	return ret;

}

// extract a double from part of a message
template<>
inline double extract<object_t::FLOAT>(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
	size_t count = 0;
	auto ret{std::stod(msg.substr(pos), &count)};
	pos += count;
	return ret;
}

// extract a string from part of a message
template<>
inline std::string extract<object_t::STR>(const std::string& msg, size_t& pos)
{
	ensure_not_end(msg, pos);
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

// extract exactly a single word from the rest of a message
// throw if there is more than one word
static inline std::string extract_single_word(const std::string& msg, size_t& pos)
{
	std::string word = extract<object_t::STR>(msg, pos);
	ensure_end(msg, pos);
	return word;
}

std::string InstructionServer::prepare(const std::string &msg) {
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
				dispatch_set(socket, msg, pos);
				break;
			}
			case operation_t::DEL:
			{
				instruction_del(socket, extract_single_word(msg, pos));
				break;
			}
			case operation_t::PERSIST:
			{
				ensure_end(msg, pos);
				instruction_persist(socket);
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
			case operation_t::RESET:
			{
				ensure_end(msg, pos);
				instruction_reset(socket);
				break;
			}
			case operation_t::PING:
			{
				instruction_ping(socket, msg.substr(pos));
				break;
			}
		}
	}
	catch (const InvalidInstruction& e) {
		send(socket, server_constants::error + prepare(e.what()));
	}
}

void InstructionServer::dispatch_set(const ClientSocket &socket, const std::string &msg, size_t &pos) {
	object_t obj = extract_object_t(msg, pos);
	std::string key = extract<object_t::STR>(msg, pos);
	switch (obj) {
		case object_t::STR:{
			auto value {extract<object_t::STR>(msg, pos)};
			ensure_end(msg, pos);
			instruction_set(socket, key, value);
			break;
		}
		case object_t::INT:{
			auto value {extract<object_t::INT>(msg, pos)};
			ensure_end(msg, pos);
			instruction_set(socket, key, value);
			break;
		}
		case object_t::FLOAT:{
			auto value {extract<object_t::FLOAT>(msg, pos)};
			ensure_end(msg, pos);
			instruction_set(socket, key, value);
			break;
		}
	}
}

} // namespace vanity
