#include <iostream>

#include "instruction_server.h"
#include "socket/socket_server.h"


namespace vanity {

/*
 * Top Level server
 */
class Server : public SocketServer, public InstructionServer
{
public:
	// a message was received from a client
	void handle(const std::string& msg, const ClientSocket& socket) override
	{
		std::cout << "Received: " << msg << std::endl;
		InstructionServer::handle(msg, socket);
	}

	// a get instruction was received from a client
	void instruction_get(const ClientSocket& socket, std::string key, std::string value) override
	{
		std::string msg = "GET Key: " + key + ", Value: " + value + "\n";
		std::cout << msg;
		socket.write(msg);
	}

	// a set instruction was received from a client
	void instruction_set(const ClientSocket& socket, std::string key, std::string value) override
	{
		std::string msg = "SET Key: " + key + ", Value: " + value + "\n";
		std::cout << msg;
		socket.write(msg);
	}

	// a del instruction was received from a client
	void instruction_del(const ClientSocket& socket, std::string key) override
	{
		std::string msg = "DEL Key: " + key + ", Value: " + "\n";
		std::cout << msg;
		socket.write(msg);
	}
};

} // namespace vanity


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