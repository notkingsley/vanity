//
// Created by kingsli on 10/26/23.
//

#ifndef VANITY_RESPONSE_H
#define VANITY_RESPONSE_H

#include <netinet/in.h>
#include <string>
#include <queue>

namespace vanity {

/*
 * A Response allows a dynamically efficient method to compose a response
 */
class Response
{
private:
	// the size of the length field
	static constexpr auto M_LENGTH_SIZE = sizeof(decltype(htonl(0)));

	// the response data
	std::string m_data;

public:
	// default constructor
	Response();

	// create a Response
	explicit Response(const std::string& data);

	// destructively extract the response data
	std::string&& extract_data();

	// add data to the response
	Response& operator<<(const std::string& data);

	// add data to the response
	Response& operator<<(const char* data);
};

} // namespace vanity

#endif //VANITY_RESPONSE_H
