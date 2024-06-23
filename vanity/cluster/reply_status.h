//
// Created by kingsli on 6/23/24.
//

#ifndef VANITY_REPLY_STATUS_H
#define VANITY_REPLY_STATUS_H

namespace vanity {

// the possible statuses of a reply message
enum class ReplyStatus : uint {
	OK,
	ERR,
	DENIED,
	REDIRECT,

	MAX_STATUS,
};

const std::initializer_list<std::pair<ReplyStatus, std::string>> REPLY_STATUS_STRINGS {
	{ReplyStatus::OK,       "OK"},
	{ReplyStatus::ERR,      "ERR"},
	{ReplyStatus::DENIED,   "DENIED"},
	{ReplyStatus::REDIRECT, "REDIRECT"},
};

} // namespace vanity

#endif //VANITY_REPLY_STATUS_H
