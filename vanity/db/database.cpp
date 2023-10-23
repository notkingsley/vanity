//
// Created by kingsli on 9/24/23.
//

#include "database.h"


namespace vanity::db {

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

// write a db_data_type to the output stream
template<>
void write<db_data_type>(std::ofstream &out, const db_data_type& value)
{
	// int8_t saves us 7 bytes per db_data_type since we'll never have > 256 types
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

// read a db_data_type from the input stream
template<>
db_data_type read<db_data_type>(std::ifstream &in)
{
	switch (read<int8_t>(in)) {
		case 0:
			return read<db_index_t<0>>(in);
		case 1:
			return read<db_index_t<1>>(in);
		case 2:
			return read<db_index_t<2>>(in);
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
	db_key_type first = read<db_key_type>(in);
	db_data_type second = read<db_data_type>(in);
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

auto Database::get(const key_type& key) -> std::optional<const data_type> {
	if (m_data.contains(key))
		return m_data.at(key);
	return std::nullopt;
}

void Database::set(const key_type& key, const data_type& value) {
	m_data[key] = value;
}

bool Database::del(const key_type& key) {
	return m_data.erase(key);
}

void Database::reset() {
	m_data.clear();
}

std::optional<int> Database::type(const Database::key_type &key) {
	if (m_data.contains(key))
		return m_data.at(key).index();
	return std::nullopt;
}

std::optional<int_t> Database::incr_int(const Database::key_type &key, int_t value) {
	if (m_data.contains(key)) {
		if (std::holds_alternative<int_t>(m_data.at(key))){
			auto& val = std::get<int_t>(m_data.at(key));
			val += value;
			return val;
		}
		else
			return std::nullopt;
	}
	else {
		m_data[key] = value;
		return value;
	}
}

std::optional<float_t> Database::incr_float(const Database::key_type &key, float_t value) {
	if (m_data.contains(key)) {
		if (std::holds_alternative<float_t>(m_data.at(key))){
			auto& val = std::get<float_t>(m_data.at(key));
			val += value;
			return val;
		}
		else
			return std::nullopt;
	}
	else {
		m_data[key] = value;
		return value;
	}
}

} // namespace vanity::db