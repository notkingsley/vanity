#include <iostream>

#include "server.h"


int main()
{
	try{
		vanity::Server server{};
		server.listen(9952);
		server.start();
	}
	catch (vanity::SocketError& e){
		std::cout << e.what() << e.get_errno() << std::endl;
	}
	return 0;
}