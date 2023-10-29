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

// write a list to the output stream
template<>
void write<list_t>(std::ofstream &out, const list_t& value)
{
	write(out, value.size());
	for (const auto& s : value)
		write(out, s);
}

// read a list from the input stream
template<>
list_t read<list_t>(std::ifstream &in)
{
	list_t list{};
	std::streamsize size = read<std::streamsize>(in);
	for (std::streamsize i = 0; i < size; ++i)
		list.push_back(read<string_t>(in));
	return list;
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
		case 3:
			return write(out, std::get<3>(value));
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
		case 3:
			return read<db_index_t<3>>(in);
		default:
			throw std::runtime_error("invalid type");
	}
}

// write a value to the output stream
template<>
void write<db_pair_type>(std::ofstream &out, const db_pair_type& value)
{
	write(out, value.first);
	write(out, value.second);
}

// read a pair from the input stream
template<>
db_pair_type read<db_pair_type>(std::ifstream &in)
{
	db_key_type first = read<db_key_type>(in);
	db_data_type second = read<db_data_type>(in);
	return std::make_pair(first, second);
}

Database &Database::operator=(Database &&other) noexcept
{
	BaseDatabase::operator=(std::move(other));
	return *this;
};

Database::Database(Database &&other) noexcept
		: BaseDatabase(std::move(other)) { };

void Database::persist(std::ofstream &out) const{
	write(out, m_data.size());
	for (const auto& pair : m_data)
		write<db_pair_type>(out, pair);
}

Database Database::from(std::ifstream &in) {
	Database db;

	size_t size = read<size_t>(in);
	for (size_t i = 0; i < size; ++i)
		db.m_data.insert(read<db_pair_type>(in));

	return db;
}

} // namespace vanity::db