//
// Created by kingsli on 9/11/23.
//

#ifndef VANITY_DATABASE_H
#define VANITY_DATABASE_H

#include <fstream>
#include <string>
#include <exception>
#include <unordered_map>


namespace vanity{

/*
 * A DatabaseException is thrown when an error occurs
 */
class DatabaseException : std::exception
{
private:
	std::string m_msg;

public:
	explicit DatabaseException(std::string msg) : m_msg{std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

/*
 * A DatabaseKeyNotFoundException is thrown when a key is not found
 */
class DatabaseKeyNotFoundException : DatabaseException {
public:
	explicit DatabaseKeyNotFoundException(std::string msg) : DatabaseException(std::move(msg)) {}
};

/*
 * A StringDatabase is an abstraction for a key value database
 */
class StringDatabase
{
private:
	// the key value store
	std::unordered_map<std::string, std::string> m_data;

public:
	using this_type = StringDatabase;

	// create a key value database
	StringDatabase() = default;

	// destroy the key value database
	~StringDatabase() = default;

	// no copy
	StringDatabase(const StringDatabase&) = delete;
	StringDatabase& operator=(const StringDatabase&) = delete;

	// move constructor
	StringDatabase(StringDatabase&& other) noexcept = default;
	StringDatabase& operator=(StringDatabase&& other) noexcept = default;

	// persist the database to a file stream
	void persist(std::ofstream &out);

	// load the database from a file stream
	static this_type from(std::ifstream &in);

	// check if the key exists
	bool has(const std::string& key) const;

	// get the value for a key
	const std::string& get(const std::string& key);

	// set the value for a key
	void set(const std::string& key, const std::string& value);

	// delete the value for a key
	bool del(const std::string& key);
};

} // namespace vanity

#endif //VANITY_DATABASE_H
