//
// Created by kingsli on 10/31/23.
//

#ifndef VANITY_SERIALIZE_H
#define VANITY_SERIALIZE_H

#include <list>
#include <optional>
#include <string>
#include <variant>
#include <vector>


namespace vanity {

// convert a type to a string
template <typename T>
struct type_to_string;

template <>
struct type_to_string<int64_t> {
	static constexpr const char* value = ":INT ";
};

template <>
struct type_to_string<double> {
	static constexpr const char* value = ":FLOAT ";
};

template <>
struct type_to_string<std::string> {
	static constexpr const char* value = ":STR ";
};

template <>
struct type_to_string<std::monostate> {
	static constexpr const char* value = ":NULL ";
};

template <typename T>
struct type_to_string<std::vector<T>> {
static constexpr const char* value = ":ARR ";
};

template <>
struct type_to_string<std::list<std::string>> {
static constexpr const char* value = ":LIST ";
};

// serialize a type to a string
template<typename T>
constexpr auto serialize_type()
{
	return type_to_string<T>::value;
}

// serialize a string object to a string
void serialize(const std::string& data, std::string& str);

// serialize an int object to a string
void serialize(int64_t data, std::string& str);

// serialize a size_t object to a string
void serialize(size_t data, std::string& str);

// serialize a double object to a string
void serialize(double data, std::string& str);

// serialize a primitive variant to a string
void serialize(const std::variant<std::string, int64_t , double, std::list<std::string>>& data, std::string& str);

// serialize a list to a string
void serialize(const std::list<std::string>& data, std::string& str);

// serialize an optional object to a string
template<typename T>
void serialize(const std::optional<T>& data, std::string& str)
{
	if (data.has_value())
		serialize(data.value(), str);
	else
		str += serialize_type<std::monostate>();
}

// serialize a vector to a string
template<typename T>
void serialize(const std::vector<T>& data, std::string& str)
{
	str += serialize_type<std::vector<T>>();
	str += '(' + std::to_string(data.size()) + ")";

	str += '[';
	for (const auto& s : data)
		serialize(s, str);
	str += ']';
}

// serialize some object to a string
template<typename T>
std::string serialize(const T& data)
{
	std::string ret {};
	serialize(data, ret);
	return ret;
}

} // namespace vanity

#endif //VANITY_SERIALIZE_H
