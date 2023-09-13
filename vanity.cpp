#include <iostream>

#include "socket/socket_server.h"


namespace vanity {

/*
 * Top Level server
 */
class Server : public vanity::SocketServer
{
	// an instruction has been received from a client
	void handle(const instruction_t& instruction, const ClientSocket& socket) override;

	void handle(const std::string &msg, const ClientSocket &socket) override;
};

void Server::handle(const instruction_t &instruction, const ClientSocket &socket) {
	std::string msg;
	switch (instruction.type) {
		case operation_t::GET:
		{
			auto *get_data = static_cast<get_data_t *>(instruction.data.get());
			msg  = "GET: Key: " + get_data->key + ", Value: " + get_data->value + "\n";
			break;
		}
		case operation_t::SET:
		{
			auto *set_data = static_cast<set_data_t *>(instruction.data.get());
			msg = "SET: Key: " + set_data->key + ", Value: " + set_data->value + "\n";
			break;
		}
		case operation_t::DEL:
		{
			auto *del_data = static_cast<del_data_t *>(instruction.data.get());
			msg = "DEL: Key: " + del_data->key + "\n";
			break;
		}
	}
	std::cout << msg;
	socket.write(msg);
}

// a message was received from a client
void Server::handle(const std::string& msg, const ClientSocket& socket)
{
	try {
		handle(parse_instruction(msg), socket);
	} catch (const InvalidInstruction& e) {
		std::string err = "Invalid instruction: " + std::string{e.what()} + "\n";
		socket.write(err);
		std::cerr << err << std::endl;
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