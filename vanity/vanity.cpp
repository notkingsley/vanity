#include <iostream>

#include "arguments.h"
#include "server.h"


int main(int argc, char **argv)
{
	vanity::Arguments args{argc, argv};

	uint32_t port = 9955;
	if (args.has_kwarg("port"))
		port = std::stoi(args.get_kwarg("port"));

	try{
		vanity::Server server{};
		server.listen(port);
		server.start();
	}
	catch (vanity::SocketError& e){
		std::cout << e.what() << e.get_errno() << std::endl;
	}
	return 0;
}