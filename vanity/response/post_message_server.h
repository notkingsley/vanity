//
// Created by kingsli on 5/2/24.
//

#ifndef VANITY_POST_MESSAGE_SERVER_H
#define VANITY_POST_MESSAGE_SERVER_H

#include <mutex>
#include <optional>
#include <unordered_map>

#include "abstract_server.h"
#include "post_message.h"


namespace vanity {

/*
 * A PostMessageServer lets us send messages to a peer
 * and track the messages we've sent
 */
class PostMessageServer : public virtual AbstractServer
{
private:
	// the operations of expected replies
	std::unordered_map<msg_id_t, peer_op_t> m_expected_ops;

	// the mutex for the expected replies
	std::mutex m_expected_ops_mutex;

	// the next message id
	msg_id_t m_next_id = 0;

	// the mutex for the next id
	std::mutex m_next_id_mutex;

	// add a message to the expected ops
	void expect_op(msg_id_t id, peer_op_t op);

	// get the next message id
	msg_id_t next_id();

	// create a new PostMessage and register the expected op
	PostMessage new_message(peer_op_t op);

	// create a new PostMessage to be sent as a plain user
	// this is different from new_message in that the message
	// created doesn't have the POST message prefix
	// and the op comes before the id
	PostMessage new_plain_message(peer_op_t op);

protected:
	// get the expected ops for a message id
	// also removes the expected op
	std::optional<peer_op_t> expected_op(msg_id_t id);

public:
	// compose and send a message to a peer
	void post(Client& peer, peer_op_t op);

	// compose and send a message to a peer
	template<typename ...Args>
	void post(Client& peer, peer_op_t op, Args&&... args) {
		auto msg = new_message(op);
		send(peer, (msg.serialize(args), ...).move());
	}

	// compose and send a message to a peer as a plain user
	// this really exists to send the initial PEER_AUTH message
	// to a peer that does not yet recognize us,
	// so op is always PEER_AUTH
	template<typename ...Args>
	void post_plain(Client& peer, peer_op_t op, Args&&... args) {
		auto msg = new_plain_message(op);
		send(peer, (msg.serialize(args), ...).move());
	}
};

} // namespace vanity

#endif //VANITY_POST_MESSAGE_SERVER_H
