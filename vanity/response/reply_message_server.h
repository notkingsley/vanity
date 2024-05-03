//
// Created by kingsli on 5/3/24.
//

#ifndef VANITY_REPLY_MESSAGE_SERVER_H
#define VANITY_REPLY_MESSAGE_SERVER_H

#include "abstract_server.h"
#include "peer_operations.h"
#include "reply_message.h"


namespace vanity {

/*
 * A ReplyMessageServer lets us send replies to a peer
 */
class ReplyMessageServer : public virtual AbstractServer
{
public:
	// reply to a message with a string
	void reply(Client& peer, msg_id_t id, const std::string& data);
};

} // namespace vanity

#endif //VANITY_REPLY_MESSAGE_SERVER_H
