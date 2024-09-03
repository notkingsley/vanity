//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_INFO_H
#define VANITY_SESSION_INFO_H

#include <chrono>
#include <memory>
#include <optional>
#include <set>
#include <string>

#include "db/trn_constants.h"


namespace vanity {

// a client's authentication level
enum class client_auth {
	UNKNOWN,
	USER,
	PEER, // and CLUSTER_MASTER maybe
	ADMIN,
};

const std::initializer_list<std::pair<client_auth, std::string>> CLIENT_AUTH_STRINGS{
	{client_auth::UNKNOWN, "UNKNOWN"},
	{client_auth::USER,    "USER"},
	{client_auth::ADMIN,   "ADMIN"},
	{client_auth::PEER,    "PEER"},
};

// a user's data
struct user_data_t {
	using channels_t = std::set<std::string>;

	// the pubsub channels the client is subscribed to
	channels_t channels;

	// the client's username
	std::string username;

	// index of the database in use
	uint database = 0;

	// id of the current transaction
	trn_id_t trn_id = NO_TRN_ID;
};

// a peer's data
struct peer_data_t {
	using peer_behaviour_score_t = uint;

	// the peer's address
	std::string addr;

	// the peer's id, if it has given one
	std::optional<std::string> id;

	// the peer's behaviour score
	peer_behaviour_score_t behaviour_score = 0;

	// last time the peer pulsed
	std::chrono::steady_clock::time_point last_pulse;

	// constructor
	explicit peer_data_t(std::string addr, std::optional<std::string> id)
		: last_pulse(std::chrono::steady_clock::now()), addr(std::move(addr)), id(std::move(id)) {}
};

/*
 * A session_info is a struct that contains information about a client's session
 */
struct session_info {
	// the peer's data, if the client is a peer
	std::unique_ptr<peer_data_t> peer_data;

	// the user's data, if the client is a user
	std::unique_ptr<user_data_t> user_data;

	// client auth
	client_auth auth = client_auth::UNKNOWN;
};

} // namespace vanity

#endif //VANITY_SESSION_INFO_H
