//
// Created by kingsli on 5/2/24.
//

#include "post_message_server.h"

namespace vanity {

msg_id_t PostMessageServer::next_id() {
	std::lock_guard lock(m_next_id_mutex);
	return m_next_id++;
}

void PostMessageServer::post(Client &peer, peer_op_t op) {
	send(peer, new_message(op));
}

void PostMessageServer::expect_op(msg_id_t id, peer_op_t op) {
	std::lock_guard lock(m_expected_ops_mutex);
	m_expected_ops[id] = op;
}

std::optional<peer_op_t> PostMessageServer::expected_op(msg_id_t id) {
	std::lock_guard lock(m_expected_ops_mutex);
	if (not m_expected_ops.contains(id))
		return std::nullopt;

	auto op = m_expected_ops[id];
	m_expected_ops.erase(id);
	return op;
}

PostMessage PostMessageServer::new_message(peer_op_t op) {
	auto id = next_id();
	expect_op(id, op);
	return PostMessage{}.serialize(id) << op;
}

PostMessage PostMessageServer::new_plain_message(peer_op_t op) {
	auto id = next_id();
	expect_op(id, op);
	return (PostMessage{PostMessage::as_plain_user} << op).serialize(id);
}

} // namespace vanity