#ifndef VANITY_ABSTRACT_SERVER_H
#define VANITY_ABSTRACT_SERVER_H

#include "client.h"
#include "exceptions.h"
#include "queue.h"


namespace vanity{

// the constants used by the server
struct server_constants
{
	static constexpr char const* const ok = "OK";
	static constexpr char const* const error = "ERROR";
	static constexpr char const* const null = "NULL";
	static constexpr char const* const pong = "PONG";
	static constexpr char const* const denied = "DENIED";
	static constexpr char const* const internal_error = "INTERNAL_ERROR";
};

// some type of sever event that should be completed
enum class server_event{
	socket_ready,
	persist,
	terminate,
};

/*
 * An AbstractServer defines the basic interface for a server
 */
class AbstractServer
{
protected:
	// max timeout in seconds for any blocking event
	static constexpr int M_MAX_TIMEOUT = 10;

	// events that need attention are sent through here
	queue<server_event> m_event_queue;

public:
	AbstractServer() = default;
	virtual ~AbstractServer() = default;

	// move constructor
	AbstractServer(AbstractServer&& other) noexcept = default;
	AbstractServer& operator=(AbstractServer&& other) noexcept = default;

	// a message was received from a client
	virtual void handle(const std::string& msg, Client& client) = 0;

	// send a message to a client
	virtual void send(Client& client, const std::string& msg) = 0;

	// send an OK message to a client
	virtual void send_ok(Client& client)
	{
		send(client, server_constants::ok);
	}

	// send an OK message to a client
	virtual void send_ok(Client& client, const std::string& msg)
	{
		send(client, server_constants::ok + msg);
	}

	// send an ERROR message to a client
	virtual void send_error(Client& client)
	{
		send(client, server_constants::error);
	}

	// send an ERROR message to a client
	virtual void send_error(Client& client, const std::string& msg)
	{
		send(client, server_constants::error + msg);
	}

	// send a NULL message to a client
	virtual void send_null(Client& client)
	{
		send(client, server_constants::null);
	}

	// send a NULL message to a client
	virtual void send_null(Client& client, const std::string& msg)
	{
		send(client, server_constants::null + msg);
	}

	// send a PONG message to a client
	virtual void send_pong(Client& client)
	{
		send(client, server_constants::pong);
	}

	// send a PONG message to a client
	virtual void send_pong(Client& client, const std::string& msg)
	{
		send(client, server_constants::pong + msg);
	}

	// send a DENIED message to a client
	virtual void send_denied(Client& client)
	{
		send(client, server_constants::denied);
	}

	// send a DENIED message to a client
	virtual void send_denied(Client& client, const std::string& msg)
	{
		send(client, server_constants::denied + msg);
	}

	// send an INTERNAL_ERROR to a client
	virtual void send_internal_error(Client& client)
	{
		send(client, server_constants::internal_error);
	}

	// send an INTERNAL_ERROR to a client
	virtual void send_internal_error(Client& client, const std::string& msg)
	{
		send(client, server_constants::internal_error + msg);
	}

	// request the server to terminate
	virtual void terminate() = 0;
};

} // namespace vanity

#endif // VANITY_ABSTRACT_SERVER_H