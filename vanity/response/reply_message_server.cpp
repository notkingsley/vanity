//
// Created by kingsli on 5/3/24.
//

#include "reply_message_server.h"

namespace vanity {

void ReplyMessageServer::reply(Client& peer, msg_id_t id, const std::string& data) {
	send(peer, (ReplyMessage{}.serialize(id) << data).move());
}

} // namespace vanity