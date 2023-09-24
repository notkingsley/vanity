//
// Created by kingsli on 9/24/23.
//

#include "database.h"

namespace vanity {

static void write_size(std::ofstream &out, size_t size)
{
	out.write(reinterpret_cast<const char *>(&size), sizeof(size));
}

static void write_string(std::ofstream &out, const std::string &str)
{
	write_size(out, str.size());
	out.write(str.data(), str.size());
}

static void write_pair(std::ofstream &out, const std::pair<std::string, std::string> &my_pair)
{
	write_string(out, my_pair.first);
	write_string(out, my_pair.second);
}

static std::streamsize read_size(std::ifstream &in)
{
	std::streamsize size;
	in.read(reinterpret_cast<char *>(&size), sizeof(size));
	return size;
}

static std::string read_string(std::ifstream &in)
{
	std::streamsize size = read_size(in);
	std::string str{};
	str.resize(size);
	in.read(str.data(), size);
	return str;
}

static std::pair<std::string, std::string> read_pair(std::ifstream &in)
{
	std::string first = read_string(in);
	std::string second = read_string(in);
	return std::make_pair(first, second);
}

void StringDatabase::persist(std::ofstream &out) const{
	write_size(out, m_data.size());
	for (const auto &my_pair : m_data)
		write_pair(out, my_pair);
}

auto StringDatabase::from(std::ifstream &in) -> this_type {
	this_type db;

	size_t size = read_size(in);
	for (size_t i = 0; i < size; ++i)
		db.m_data.insert(read_pair(in));

	return db;
}

bool StringDatabase::has(const std::string &key) const {
	return m_data.contains(key);
}

const std::string &StringDatabase::get(const std::string &key) {
	if (has(key)) {
		return m_data.at(key);
	}
	throw DatabaseKeyNotFoundException("Key not found");
}

void StringDatabase::set(const std::string &key, const std::string &value) {
	m_data[key] = value;
}

bool StringDatabase::del(const std::string &key) {
	return m_data.erase(key);
}

void StringDatabase::reset() {
	m_data.clear();
}

} // namespace vanity