#include "server.h"
#include "signals.h"


int main(int argc, char **argv)
{
	vanity::Config config{{argc, argv}};
	vanity::Server server{config};

	vanity::signal::set_server(&server);
	server.run();
}