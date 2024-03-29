#include "server.h"
#include "signals.h"


int main(int argc, char **argv)
{
	vanity::Server server{{{argc, argv}}};
	vanity::signal::set_server(&server);
	server.run();
	return 0;
}
