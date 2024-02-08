#include "server.h"
#include "signals.h"


int main(int argc, char **argv)
{
	using vanity::Config, vanity::Server;
	Server server{Config{{argc, argv}}};

	vanity::signal::set_server(&server);
	server.run();

	return 0;
}