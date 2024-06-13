//
// Created by kingsli on 5/3/24.
//

#ifndef VANITY_REPLY_MESSAGE_H
#define VANITY_REPLY_MESSAGE_H

#include <unordered_set>

#include "sendable.h"


namespace vanity {

/*
 * A ReplyMessage is a message that is sent in response to a request
 */
class ReplyMessage : public Sendable
{
public:
	// create a new ReplyMessage
	ReplyMessage();

	// add a string to the message
	ReplyMessage& operator<<(const std::string& data);

	// add a const char* to the message
	ReplyMessage& operator<<(const char* data);

	// add a char to the message
	ReplyMessage& operator<<(char data);

	// serialize a string to the message
	ReplyMessage& serialize(const std::string& data);

	// serialize an int64_t to the ReplyMessage
	ReplyMessage& serialize(int64_t data);

	// serialize a length to the ReplyMessage
	ReplyMessage& serialize_length(size_t length);

	// serialize a set of strings to the ReplyMessage
	ReplyMessage& serialize(const std::unordered_set<std::string>& data);

	// serialize a bool to the ReplyMessage
	ReplyMessage& serialize(bool data);

	// serialize a C-string to the ReplyMessage
	ReplyMessage& serialize(const char* data);
};

} // namespace vanity

#endif //VANITY_REPLY_MESSAGE_H
