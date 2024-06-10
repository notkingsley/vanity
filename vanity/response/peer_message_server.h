//
// Created by kingsli on 5/2/24.
//

#ifndef VANITY_PEER_MESSAGE_SERVER_H
#define VANITY_PEER_MESSAGE_SERVER_H

#include <mutex>
#include <optional>
#include <unordered_map>

#include "abstract_server.h"
#include "async_message.h"
#include "post_message.h"
#include "reply_message.h"


namespace vanity {

/*
 * A PeerMessageServer lets us send messages to a peer
 * and track the messages we've sent and reply to messages
 */
class PeerMessageServer : public virtual AbstractServer
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
	// return the new message and the id
	std::pair<PostMessage, msg_id_t> new_post(peer_op_t op);

	// create a new PostMessage to be sent as a plain user
	// this is different from new_post in that the message
	// created doesn't have the POST message prefix
	// and the op comes before the id
	std::pair<PostMessage, msg_id_t> new_plain_post(peer_op_t op);

	// create a new ReplyMessage with the given id
	// return the new message
	static ReplyMessage new_reply(msg_id_t id);

	// create a new AsyncMessage and return it
	static AsyncMessage new_async(async_op_t op);

protected:
	// context passed to handlers
	struct Context {
		msg_id_t msg_id;
		Client &client;
	};

	// get the expected ops for a message id
	// also removes the expected op
	std::optional<peer_op_t> expected_op(msg_id_t id);

public:
	// compose and send a message to a peer
	msg_id_t post(Client& peer, peer_op_t op);

	// compose and send a message to a peer
	template<typename ...Args>
	msg_id_t post(Client& peer, peer_op_t op, Args&&... args) {
		auto [msg, id] = new_post(op);
		send(peer, (msg.serialize(args), ...).move());
		return id;
	}

	// compose and send a message to a peer as a plain user
	// this really exists to send the initial PEER_AUTH message
	// to a peer that does not yet recognize us,
	// so op is always PEER_AUTH
	template<typename ...Args>
	msg_id_t post_plain(Client& peer, peer_op_t op, Args&&... args) {
		auto [msg, id] = new_plain_post(op);
		send(peer, (msg.serialize(args), ...).move());
		return id;
	}

	// reply to a message with some data
	template<typename T>
	void reply(Context& ctx, const T& data) {
		send(ctx.client, new_reply(ctx.msg_id).serialize(data).move());
	}

	// compose and send an async message to a peer
	void send_async(Client& peer, async_op_t op);
};

} // namespace vanity

#endif //VANITY_PEER_MESSAGE_SERVER_H
