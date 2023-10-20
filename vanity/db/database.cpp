//
// Created by kingsli on 9/24/23.
//

#include "database.h"

namespace vanity::db {

using mapped_type = Database::mapped_type;

using pair_type = std::pair<string_t, mapped_type>;

template<size_t I>
using index_t = typename std::variant_alternative_t<I, mapped_type>;


// write something to an output stream
template<typename T>
std::enable_if_t<!std::is_arithmetic_v<T>, void> write(std::ofstream &out, const T& value);

// read something from an input stream
template<typename T>
std::enable_if_t<!std::is_arithmetic_v<T>, T> read(std::ifstream &in);

// write an arithmetic value to the output stream
template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, void> write(std::ofstream &out, const T& value)
{
	out.write(reinterpret_cast<const char *>(&value), sizeof(T));
}

// read an arithmetic value from the input stream
template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, T> read(std::ifstream &in)
{
	T value;
	in.read(reinterpret_cast<char *>(&value), sizeof(T));
	return value;
}

// write a string to the output stream
template<>
void write<string_t>(std::ofstream &out, const string_t& value)
{
	write(out, value.size());
	out.write(value.data(), value.size());
}

// read a string from the input stream
template<>
string_t read<string_t>(std::ifstream &in)
{
	std::streamsize size = read<std::streamsize>(in);
	string_t str{};
	str.resize(size);
	in.read(str.data(), size);
	return str;
}

// write a mapped_type to the output stream
template<>
void write<mapped_type>(std::ofstream &out, const mapped_type& value)
{
	// int8_t saves us 7 bytes per mapped_type since we'll never have > 256 types
	auto index = static_cast<int8_t>(value.index());
	write(out, index);
	switch (index) {
		case 0:
			return write(out, std::get<0>(value));
		case 1:
			return write(out, std::get<1>(value));
		case 2:
			return write(out, std::get<2>(value));
		default:
			throw std::runtime_error("invalid type");
	}
}

// read a mapped_type from the input stream
template<>
mapped_type read<mapped_type>(std::ifstream &in)
{
	switch (read<int8_t>(in)) {
		case 0:
			return read<index_t<0>>(in);
		case 1:
			return read<index_t<1>>(in);
		case 2:
			return read<index_t<2>>(in);
		default:
			throw std::runtime_error("invalid type");
	}
}

// write a value to the output stream
template<>
void write<pair_type>(std::ofstream &out, const pair_type& value)
{
	write(out, value.first);
	write(out, value.second);
}

// read a pair from the input stream
template<>
pair_type read<pair_type>(std::ifstream &in)
{
	string_t first = read<string_t>(in);
	mapped_type second = read<mapped_type>(in);
	return std::make_pair(first, second);
}

void Database::persist(std::ofstream &out) const{
	write(out, m_data.size());
	for (const auto& pair : m_data)
		write<pair_type>(out, pair);
}

Database Database::from(std::ifstream &in) {
	Database db;

	size_t size = read<size_t>(in);
	for (size_t i = 0; i < size; ++i)
		db.m_data.insert(read<pair_type>(in));

	return db;
}

bool Database::has(const key_type& key) const {
	return m_data.contains(key);
}

auto Database::get(const key_type& key) -> std::optional<const mapped_type> {
	if (m_data.contains(key))
		return m_data.at(key);
	return std::nullopt;
}

void Database::set(const key_type& key, const mapped_type& value) {
	m_data[key] = value;
}

bool Database::del(const key_type& key) {
	return m_data.erase(key);
}

void Database::reset() {
	m_data.clear();
}

} // namespace vanity::db