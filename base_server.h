#ifndef VANITY_BASE_SERVER_H
#define VANITY_BASE_SERVER_H

#include <iostream>

#include "socket.h"


namespace vanity{

class BaseServer
{
public:
	BaseServer() {};
	~BaseServer() {};

	virtual void handle(std::string msg, const ClientSocket& sock)
	{
		std::cout << "Message: " << msg << '\n';
	}
};

}

#endif // VANITY_BASE_SERVER_H