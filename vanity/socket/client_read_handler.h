//
// Created by kingsli on 4/4/24.
//

#ifndef VANITY_CLIENT_READ_HANDLER_H
#define VANITY_CLIENT_READ_HANDLER_H

#include <sys/epoll.h>

#include "client/client_manager.h"
#include "socket_read_handler.h"


namespace vanity::socket {

/*
 * A ClientReadHandler is the interface for read handlers that
 * deal with clients and need a ClientManager instead of just a ReadManager
*/
class ClientReadHandler : public SocketReadHandler
{
public:
	// the registered event has occurred
	// return true if the handler should be kept, false otherwise
	virtual void ready(ClientManager& manager) = 0;

private:
	void ready(ReadManager& manager) override {
		throw std::runtime_error("ClientReadHandler does not support ReadManager");
	};
};

} // namespace vanity::socket

#endif //VANITY_CLIENT_READ_HANDLER_H
