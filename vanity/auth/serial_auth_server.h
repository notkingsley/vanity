//
// Created by kingsli on 10/15/23.
//

#ifndef VANITY_SERIAL_AUTH_SERVER_H
#define VANITY_SERIAL_AUTH_SERVER_H

#include <variant>

#include "auth_server.h"
#include "utils/task_serializer.h"

namespace vanity {

// define the types used by the serial auth server
struct serial_auth_server_types {
	enum class task_type {
		AUTH,
		ADD_USER,
		EDIT_USER,
		DEL_USER,
		CHANGE_PASSWORD,
	};

	using auth_type = std::tuple<Client&, std::string, std::string>;
	using add_user_type = std::tuple<Client&, std::string, std::string>;
	using edit_user_type = std::tuple<Client&, std::string, client_auth>;
	using del_user_type = std::tuple<Client&, std::string>;
	using change_password_type = std::tuple<Client&, std::string>;

	using data_type = std::variant<
		auth_type,
		edit_user_type,
		del_user_type
	>;
};

/*
 * A SerialAuthServer serializes all auth operations to a single thread
 */
class SerialAuthServer :
	public virtual AuthServer,
	private serial_auth_server_types,
	private TaskSerializer<
		serial_auth_server_types::task_type,
		serial_auth_server_types::data_type,
		void
	>
{
private:
	using serial_auth_server_types::task_type;
	using serial_auth_server_types::data_type;
	using serial_auth_server_types::auth_type;
	using serial_auth_server_types::add_user_type;
	using serial_auth_server_types::edit_user_type;
	using serial_auth_server_types::del_user_type;
	using serial_auth_server_types::change_password_type;

	// perform a task that was sent from another thread
	void perform(task_type t, data_type data, std::promise<void>) override;

public:
	// start the server
	void start();

	// stop the server
	void stop();

	// an add_user request was received from a client
	void request_add_user(Client& client, const std::string& username, const std::string& password) override;

	// an edit_user request was received from a client
	void request_edit_user(Client& client, const std::string& username, client_auth auth_level) override;

	// a del_user request was received from a client
	void request_del_user(Client& client, const std::string& username) override;

	// an auth request was received from a client
	void request_auth(Client& client, const std::string& username, const std::string& password) override;

	// a change_password request was received from a client
	void request_change_password(Client& client, const std::string& new_password) override;
};

} // namespace vanity

#endif //VANITY_SERIAL_AUTH_SERVER_H
