//
// Created by kingsli on 10/31/23.
//

#include <stdexcept>

#include "serialize.h"


namespace vanity {

inline void serialize_body(const std::string &data, std::string& str) {
	str.reserve(str.size() + data.size() + 10);
	str += '(' + std::to_string(data.size()) + ")";
	str += data;
}

void serialize(const std::string &data, std::string& str) {
	str.reserve(str.size() + data.size() + 10);
	str += serialize_type<std::string>();
	serialize_body(data, str);
}

void serialize(int64_t data, std::string& str) {
	str += serialize_type<int64_t>();
	str += std::to_string(data);
}

void serialize(size_t data, std::string& str) {
	serialize(int64_t(data), str);
}

void serialize(double data, std::string& str) {
	str += serialize_type<double>();
	str += std::to_string(data);
}

void serialize(const primary_serialize_type& data, std::string& str) {
	switch (data.index()) {
		case 0:
		{
			serialize(std::get<0>(data), str);
			break;
		}
		case 1:
		{
			serialize(std::get<1>(data), str);
			break;
		}
		case 2:
		{
			serialize(std::get<2>(data), str);
			break;
		}
		case 3:
		{
			serialize(std::get<3>(data), str);
			break;
		}
		case 4:
		{
			serialize(std::get<4>(data), str);
			break;
		}
		default:
			throw std::runtime_error("invalid type");
	}
}

void serialize(const std::list<std::string>& data, std::string& str) {
	str += serialize_type<std::list<std::string>>();
	str += '(' + std::to_string(data.size()) + ")";

	str += '[';
	for (const auto& s : data)
		serialize_body(s, str);
	str += ']';
}

void serialize(const std::unordered_set<std::string>& data, std::string& str) {
	str += serialize_type<std::unordered_set<std::string>>();
	str += '(' + std::to_string(data.size()) + ")";

	str += '{';
	for (const auto &s: data)
		serialize_body(s, str);
	str += '}';
}

void serialize(bool data, std::string& str) {
	str += serialize_type<bool>();
	str += data ? "true" : "false";
}

} // namespace vanity