//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_REQUEST_SERVER_H
#define VANITY_REQUEST_SERVER_H


#include "abstract_server.h"


namespace vanity {

// the message received was invalid
class InvalidRequest : std::exception
{
private:
	std::string m_msg;

public:
	explicit InvalidRequest(std::string msg) : m_msg{"Invalid request: " + std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

// to annotate a return value with the type
template <typename T>
struct type_to_string;

template <>
struct type_to_string<int64_t> {
	static constexpr const char* value = ":INT ";
};

template <>
struct type_to_string<double> {
	static constexpr const char* value = ":FLOAT ";
};

template <>
struct type_to_string<std::string> {
	static constexpr const char* value = ":STR ";
};

/*
 * A RequestServer allows to process incoming requests
 * and dispatch them to the appropriate handler
 */
class RequestServer : public virtual AbstractServer
{
public:
	// prepare a string to be sent
	static std::string prepare(const std::string& msg);

	// prepare a numeric value to be sent
	template<typename T>
	static std::enable_if_t<std::is_arithmetic_v<T>, std::string> prepare(T value){
		return type_to_string<T>::value + std::to_string(value);
	}

	// a message was received from a client
	void handle(const std::string& msg, const Client& client) override;

	// a get request was received from a client
	virtual void request_get(const Client& client, const std::string& key) = 0;

	// a set request was received from a client
	virtual void request_set(const Client& client, const std::string& key, const std::string& value) = 0;

	// a set request was received from a client
	virtual void request_set(const Client& client, const std::string& key, const int64_t& value) = 0;

	// a set request was received from a client
	virtual void request_set(const Client& client, const std::string& key, const double& value) = 0;

	// a del request was received from a client
	virtual void request_del(const Client& client, const std::string& key) = 0;

	// a persist request was received from a client
	virtual void request_persist(const Client& client) = 0;

	// a reset request was received from a client
	virtual void request_reset(const Client& client) = 0;

	// an exit request was received from a client
	virtual void request_exit(const Client& client) {
		throw DestroyClient{};
	};

	// a terminate request was received from a client
	virtual void request_terminate(const Client& client) {
		terminate();
	};

	// a ping request was received from a client
	virtual void request_ping(const Client& client, const std::string& msg) {
		send(client, server_constants::pong + msg);
	};

private:
	// convenience function to dispatch a set request by type
	void dispatch_set(const Client& client, const std::string& msg, size_t& pos);
};

} // namespace vanity

#endif //VANITY_REQUEST_SERVER_H
