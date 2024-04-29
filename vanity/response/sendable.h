//
// Created by kingsli on 4/29/24.
//

#ifndef VANITY_SENDABLE_H
#define VANITY_SENDABLE_H

#include <netinet/in.h>
#include <string>

namespace vanity {

/*
 * A Sendable is an object that can be sent over the network
 * like a Response or a PostMessage or ReplyMessage
 */
class Sendable
{
private:
	// the size of the length field
	static constexpr auto M_LENGTH_SIZE = sizeof(decltype(htonl(0)));

	// the data to be sent
	std::string m_data;

public:
	// default constructor
	Sendable();

	// destructively extract the sendable data
	std::string&& extract_data();

	// obtain a view of the (current) body (without the length field)
	std::string_view body() const;

	// reserve space in the sendable
	void reserve(size_t size);

	// return this as a rvalue
	Sendable&& move();

	// add a string to the sendable
	Sendable& operator<<(const std::string& data);

	// add a C string to the sendable
	Sendable& operator<<(const char* data);

	// add a char to the sendable
	Sendable& operator<<(char data);
};

} // namespace vanity

#endif //VANITY_SENDABLE_H
