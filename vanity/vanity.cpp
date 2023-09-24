#include <iostream>

#include "arguments.h"
#include "server.h"


// extract a server config from the command line arguments
static inline vanity::ServerConfig extract_config(const vanity::Arguments& args)
{
	vanity::ServerConfig config{};

	if (args.has_kwarg("persist_file") and args.has("no_persist")){
		std::cout << "Error: --no-persist and --persist-file are mutually exclusive" << std::endl;
		exit(1);
	}
	else if (args.has("no_persist"))
		config.db_file = std::nullopt;
	else if (args.has_kwarg("persist_file"))
		config.db_file = args.get_kwarg("persist_file");
	else
		config.db_file = "db.vanity";

	if (args.has_kwarg("port"))
		config.port = std::stoi(args.get_kwarg("port"));
	else
		config.port = 9955;

	return config;
}

int main(int argc, char **argv)
{
	vanity::Arguments args{argc, argv};

	try{
		vanity::Server server{extract_config(args)};
		server.run();
	}
	catch (vanity::SocketError& e){
		std::cout << e.what() << e.get_errno() << std::endl;
	}
	return 0;
}