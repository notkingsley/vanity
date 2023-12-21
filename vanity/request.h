//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_REQUEST_H
#define VANITY_REQUEST_H

#include <string>

namespace vanity {

/*
 * A Request is a class that holds information
 * about a request string currently being processed
 */
class Request
{
private:
	// the request string
	const std::string& msg;

	// the current position in the request string
	size_t pos = 0;

public:
	// create a Request with a request string
	explicit Request(const std::string& msg) : msg(msg) {}

	// the current character in the request string
	// shorthand for msg[pos]
	char current() const {
		return msg[pos];
	}

	// returns true if the request string has not been fully processed
	// shorthand for pos < msg.size()
	bool not_end() const {
		return pos < msg.size();
	}

	// returns true if the request string has been fully processed
	// shorthand for pos >= msg.size()
	bool end() const {
		return pos >= msg.size();
	}

	// increments the current position in the request string
	// shorthand for ++pos
	size_t operator++() {
		return ++pos;
	}

	// increments the current position in the request string
	// shorthand for pos += n
	size_t operator+=(size_t n) {
		return pos += n;
	}

	// compares from the current position in the request string to another string
	// shorthand for msg.compare(pos, str.size(), str) == 0
	bool compare(const std::string& str) const {
		return msg.compare(pos, str.size(), str) == 0;

	}

	// returns a substring of the request string from the current position
	// shorthand for msg.substr(pos)
	std::string substr() const {
		return msg.substr(pos);
	}

	// returns a substring of the request string from the current position
	// shorthand for msg.substr(pos, n)
	std::string substr(size_t n) const {
		return msg.substr(pos, n);
	}

	// checks if there are up to n characters left in the request string
	// shorthand for pos + n <= msg.size()
	bool has_up_to(size_t n) const {
		return pos + n <= msg.size();
	}

	// formats the request string and position logging
	std::string format() const {
		return msg + " at " + std::to_string(pos);
	}
};

} // namespace vanity

#endif //VANITY_REQUEST_H
