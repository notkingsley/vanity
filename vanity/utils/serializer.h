//
// Created by kingsli on 1/20/24.
//

#ifndef VANITY_SERIALIZER_H
#define VANITY_SERIALIZER_H

#include <concepts>
#include <fstream>

#include "db/db/types.h"

namespace vanity::serializer {

// write something to an output stream
template<typename T>
static void write(std::ofstream &out, const T& value);

// read something from an input stream
template<typename T>
static T read(std::ifstream &in);

// write an arithmetic value to the output stream
template<typename T>
requires std::is_arithmetic_v<std::remove_reference_t<T>>
void write(std::ofstream &out, const T& value)
{
	out.write(reinterpret_cast<const char *>(&value), sizeof(T));
}

// read an arithmetic value from the input stream
template<typename T>
requires std::is_arithmetic_v<std::remove_reference_t<T>>
T read(std::ifstream &in)
{
	T value;
	in.read(reinterpret_cast<char *>(&value), sizeof(T));
	return value;
}

// write an enum to the output stream
template<typename T>
requires std::is_enum_v<std::remove_reference_t<T>>
void write(std::ofstream &out, const T& value)
{
	write(out, static_cast<std::underlying_type_t<T>>(value));
}

// read an enum from the input stream
template<typename T>
requires std::is_enum_v<std::remove_reference_t<T>>
T read(std::ifstream &in)
{
	return static_cast<T>(read<std::underlying_type_t<T>>(in));
}

// write a string to the output stream
template<>
void write<db::string_t>(std::ofstream &out, const db::string_t& value)
{
	write(out, value.size());
	out.write(value.data(), value.size());
}

// read a string from the input stream
template<>
db::string_t read<db::string_t>(std::ifstream &in)
{
	auto size = read<std::streamsize>(in);
	db::string_t str{};
	str.resize(size);
	in.read(str.data(), size);
	return str;
}

// write a list to the output stream
template<>
void write<db::list_t>(std::ofstream &out, const db::list_t& value)
{
	write(out, value.size());
	for (const auto& s : value)
		write(out, s);
}

// read a list from the input stream
template<>
db::list_t read<db::list_t>(std::ifstream &in)
{
	db::list_t list{};
	auto size = read<std::streamsize>(in);
	for (std::streamsize i = 0; i < size; ++i)
		list.push_back(read<db::string_t>(in));
	return list;
}

// write a set to the output stream
template<>
void write<db::set_t>(std::ofstream &out, const db::set_t& value)
{
	write(out, value.size());
	for (const auto& s : value)
		write(out, s);
}

// read a set from the input stream
template<>
db::set_t read<db::set_t>(std::ifstream &in)
{
	db::set_t set{};
	auto size = read<std::streamsize>(in);
	for (std::streamsize i = 0; i < size; ++i)
		set.insert(read<db::string_t>(in));
	return set;
}

// write a hash to the output stream
template<>
void write<db::hash_t>(std::ofstream &out, const db::hash_t& value)
{
	write(out, value.size());
	for (const auto& s : value) {
		write(out, s.first);
		write(out, s.second);
	}
}

// read a hash from the input stream
template<>
db::hash_t read<db::hash_t>(std::ifstream &in)
{
	db::hash_t hash{};
	auto size = read<std::streamsize>(in);
	for (std::streamsize i = 0; i < size; ++i) {
		auto key = read<db::string_t>(in);
		auto value = read<db::string_t>(in);
		hash[key] = value;
	}
	return hash;
}

// write a primary_serialize_type to the output stream
template<>
void write<db::db_data_type>(std::ofstream &out, const db::db_data_type& value)
{
	// int8_t saves us 7 bytes per primary_serialize_type since we'll never have > 256 types
	auto index = static_cast<int8_t>(value.index());
	write(out, index);
	switch (index) {
		case 0:
			return write(out, std::get<0>(value));
		case 1:
			return write(out, std::get<1>(value));
		case 2:
			return write(out, std::get<2>(value));
		case 3:
			return write(out, std::get<3>(value));
		case 4:
			return write(out, std::get<4>(value));
		case 5:
			return write(out, std::get<5>(value));
		default:
			throw std::runtime_error("invalid type");
	}
}

// read a primary_serialize_type from the input stream
template<>
db::db_data_type read<db::db_data_type>(std::ifstream &in)
{
	switch (read<int8_t>(in)) {
		case 0:
			return read<db::db_index_t<0>>(in);
		case 1:
			return read<db::db_index_t<1>>(in);
		case 2:
			return read<db::db_index_t<2>>(in);
		case 3:
			return read<db::db_index_t<3>>(in);
		case 4:
			return read<db::db_index_t<4>>(in);
		case 5:
			return read<db::db_index_t<5>>(in);
		default:
			throw std::runtime_error("invalid type");
	}
}

// write a value to the output stream
template<>
void write<db::db_pair_type>(std::ofstream &out, const db::db_pair_type& value)
{
	write(out, value.first);
	write(out, value.second);
}

// read a pair from the input stream
template<>
db::db_pair_type read<db::db_pair_type>(std::ifstream &in)
{
	db::db_key_type first = read<db::db_key_type>(in);
	db::db_data_type second = read<db::db_data_type>(in);
	return std::make_pair(first, second);
}

// write a db::time_t to the output stream
template<>
void write<db::time_t>(std::ofstream &out, const db::time_t& value)
{
	out.write(reinterpret_cast<const char *>(&value), sizeof(db::time_t));
}

// read a db::time_t from the input stream
template<>
db::time_t read<db::time_t>(std::ifstream &in)
{
	db::time_t value;
	in.read(reinterpret_cast<char *>(&value), sizeof(db::time_t));
	return value;
}

// write a db::expiry_db_pair_type to the output stream
template<>
void write<db::expiry_db_pair_type>(std::ofstream &out, const db::expiry_db_pair_type& value)
{
	write(out, value.first);
	write(out, value.second);
}

// read a db::expiry_db_pair_type from the input stream
template<>
db::expiry_db_pair_type read<db::expiry_db_pair_type>(std::ifstream &in)
{
	db::db_key_type first = read<db::db_key_type>(in);
	db::time_t second = read<db::time_t>(in);
	return std::make_pair(first, second);
}

// write an unordered_map to the output stream
template<typename K, typename V>
void write(std::ofstream &out, const std::unordered_map<K, V>& value)
{
	write(out, value.size());
	for (const auto& pair : value)
		write<std::pair<K, V>>(out, pair);
}

} // namespace vanity::serializer

#endif //VANITY_SERIALIZER_H
