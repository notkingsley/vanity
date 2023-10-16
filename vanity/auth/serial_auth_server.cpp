//
// Created by kingsli on 10/15/23.
//

#include "serial_auth_server.h"

namespace vanity {

void SerialAuthServer::perform(SerialAuthServer::task_type t, SerialAuthServer::data_type data) {
	switch (t) {
		case task_type::AUTH:
		{
			auto& [client, username, password] = std::get<auth_type>(data);
			AuthServer::request_auth(client, username, password);
			break;
		}
		case task_type::ADD_USER:
		{
			auto& [client, username, password] = std::get<add_user_type>(data);
			AuthServer::request_add_user(client, username, password);
			break;
		}
		case task_type::EDIT_USER:
		{
			auto& [client, username, auth_level] = std::get<edit_user_type>(data);
			AuthServer::request_edit_user(client, username, auth_level);
			break;
		}
		case task_type::DEL_USER:
		{
			auto& [client, username] = std::get<del_user_type>(data);
			AuthServer::request_del_user(client, username);
			break;
		}
		case task_type::CHANGE_PASSWORD:
		{
			auto& [client, password] = std::get<change_password_type>(data);
			AuthServer::request_change_password(client, password);
			break;
		}
	}
}

void SerialAuthServer::start() {
	TaskSerializer::start(M_MAX_TIMEOUT);
}

void SerialAuthServer::stop() {
	TaskSerializer::stop();
}

void
SerialAuthServer::request_add_user(const Client &client, const std::string &username, const std::string &password) {
	send_task(task_type::ADD_USER, add_user_type{client, username, password});
}

void SerialAuthServer::request_edit_user(const Client &client, const std::string &username, client_auth auth_level) {
	send_task(task_type::EDIT_USER, edit_user_type{client, username, auth_level});
}

void SerialAuthServer::request_del_user(const Client &client, const std::string &username) {
	send_task(task_type::DEL_USER, del_user_type{client, username});
}

void SerialAuthServer::request_auth(const Client &client, const std::string &username, const std::string &password) {
	send_task(task_type::AUTH, auth_type{client, username, password});
}

void SerialAuthServer::request_change_password(const Client &client, const std::string &new_password) {
	send_task(task_type::CHANGE_PASSWORD, change_password_type{client, new_password});
}


} // namespace vanity