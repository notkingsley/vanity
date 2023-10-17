//
// Created by kingsli on 10/3/23.
//

#ifndef VANITY_SIGNALS_H
#define VANITY_SIGNALS_H

#include <csignal>

#include "abstract_server.h"


namespace vanity::signal{

// server to send signals to
AbstractServer* _server = nullptr;

// signal handler
void handle(int signal)
{
	if (not _server)
		std::terminate();

	switch (signal)
	{
		case SIGINT:
		case SIGTERM:
			_server->terminate();
			break;
		default:
			std::terminate();
	}
}

// register the signal handler
void install_handlers()
{
	struct sigaction action{};
	action.sa_handler = handle;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGINT, &action, nullptr);
	sigaction(SIGTERM, &action, nullptr); // expensive
}

// set the server to send signals to, and install the signal handler
void set_server(AbstractServer* srv)
{
	_server = srv;
	install_handlers();
}

} // namespace vanity::signal

#endif //VANITY_SIGNALS_H
