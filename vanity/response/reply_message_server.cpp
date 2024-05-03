//
// Created by kingsli on 5/3/24.
//

#include "reply_message_server.h"

namespace vanity {

void ReplyMessageServer::reply(Context ctx, const std::string& data) {
	send(ctx.client, ReplyMessage{}.serialize(ctx.msg_id).serialize(data).move());
}

} // namespace vanity