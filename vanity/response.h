//
// Created by kingsli on 10/26/23.
//

#ifndef VANITY_RESPONSE_H
#define VANITY_RESPONSE_H

#include <string>
#include <queue>

#include "socket/socket.h"


namespace vanity {

/*
 * A Response allows a dynamically efficient method to build a response
 * Responses are one-time use only, and are NOT thread-safe
 */
class Response
{
private:
	// the size of the length field
	static constexpr size_t M_LENGTH_SIZE = sizeof(decltype(htonl(0)));

	// the response data
	std::string m_data;

	// the index of the next character to write
	size_t m_index = 0;

public:
	// create a Response
	explicit Response(const std::string& data = {});

	// write the response to the socket
	// do not add data to the response after calling
	// return false when done, true otherwise
	bool write(const ClientSocket& socket);

	// add data to the response
	Response& operator<<(const std::string& data);

	// add data to the response
	Response& operator<<(const char* data);
};

} // namespace vanity

#endif //VANITY_RESPONSE_H
