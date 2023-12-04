#include "parse_args.h"
#include "server.h"
#include "signals.h"


int main(int argc, char **argv)
{
	vanity::Arguments args{argc, argv};
	vanity::ServerConfig config{extract_config(args)};

	vanity::Server server{config};
	vanity::signal::set_server(&server);

	server.run();
	return 0;
}