//
// Created by kingsli on 9/11/23.
//

#ifndef VANITY_INSTRUCTION_H
#define VANITY_INSTRUCTION_H

#include <string>
#include <exception>
#include <memory>


namespace vanity{

// the message received was invalid
class InvalidInstruction : std::exception
{
private:
	std::string m_msg;

public:
	explicit InvalidInstruction(std::string  msg) : m_msg{std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

enum class operation_t{
	GET,
	SET,
	DEL,
};

// a GET instruction data
struct get_data_t
{
	std::string key;
	std::string value;
};

// a SET instruction data
struct set_data_t
{
	std::string key;
	std::string value;
};

// a DEL instruction data
struct del_data_t
{
	std::string key;
};

using unique_void_ptr = std::unique_ptr<void, void(*)(void const*)>;

template<typename T>
auto unique_void(T * ptr) -> unique_void_ptr
{
	return unique_void_ptr(ptr, [](void const * data) {
		T const * p = static_cast<T const*>(data);
		delete p;
	});
}

template<typename T, typename... Args>
auto make_unique_void(Args&&... args)
{
	return unique_void(new T(std::forward<Args>(args)...));
}

// general format for all supported instructions
struct instruction_t
{
	operation_t type;
	unique_void_ptr data;
};

// extract a word from part of a message
std::string extract_word(const std::string& msg, size_t& pos);

// extract a single word from the rest of a message
// throw if there is more than one word
std::string extract_single_word(const std::string& msg, size_t& pos);

// extract a key-value pair from the rest of a message
// throw if there is more than one pair
std::pair<std::string, std::string> extract_key_value(const std::string& msg, size_t& pos);

// parse_instruction a get instruction into a unique_void_ptr
unique_void_ptr parse_get(const std::string& msg);

// parse_instruction a set instruction into a unique_void_ptr
unique_void_ptr parse_set(const std::string& msg);

// parse_instruction a del instruction into a unique_void_ptr
unique_void_ptr parse_del(const std::string& msg);

// parse_instruction a message into an instruction
instruction_t parse_instruction(const std::string& msg);

} // namespace vanity

#endif //VANITY_INSTRUCTION_H
