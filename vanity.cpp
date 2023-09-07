#include <iostream>

#include "socket_connection_server.h"


int main()
{
	try{
		vanity::SocketServer server{};
		server.listen(9955);
		server.start();
	}
	catch (vanity::SocketError& e){
		std::cout << e.what() << std::endl;
	}
	return 0;
}