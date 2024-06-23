//
// Created by kingsli on 5/2/24.
//

#include "peer_message_server.h"

namespace vanity {

msg_id_t PeerMessageServer::next_id() {
	std::lock_guard lock(m_next_id_mutex);
	return m_next_id++;
}

msg_id_t PeerMessageServer::post(Client &peer, peer_op_t op) {
	auto [msg, id] = new_post(op);
	send(peer, msg.move());
	return id;
}

void PeerMessageServer::send_async(Client &peer, async_op_t op) {
	send(peer, new_async(op));
}

void PeerMessageServer::expect_op(msg_id_t id, peer_op_t op) {
	std::lock_guard lock(m_expected_ops_mutex);
	m_expected_ops[id] = op;
}

std::optional<peer_op_t> PeerMessageServer::expected_op(msg_id_t id) {
	std::lock_guard lock(m_expected_ops_mutex);
	if (not m_expected_ops.contains(id))
		return std::nullopt;

	auto op = m_expected_ops[id];
	m_expected_ops.erase(id);
	return op;
}

std::pair<PostMessage, msg_id_t> PeerMessageServer::new_post(peer_op_t op) {
	auto id = next_id();
	expect_op(id, op);
	return {PostMessage{}.serialize(id) << op, id};
}

std::pair<PostMessage, msg_id_t> PeerMessageServer::new_plain_post(peer_op_t op) {
	auto id = next_id();
	expect_op(id, op);
	return {(PostMessage{PostMessage::as_plain_user} << op).serialize(id), id};
}

ReplyMessage PeerMessageServer::new_reply(msg_id_t id, ReplyStatus status) {
	return ReplyMessage{}.serialize(id) << status;
}

AsyncMessage PeerMessageServer::new_async(async_op_t op) {
	return AsyncMessage{} << op;
}

} // namespace vanity
