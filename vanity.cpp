#include <iostream>

#include "server.h"


int main()
{
	try{
		vanity::Server server{};
		server.listen(9958);
		server.start();
	}
	catch (vanity::SocketError& e){
		std::cout << e.what() << std::endl;
	}
	return 0;
}