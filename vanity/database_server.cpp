//
// Created by kingsli on 9/14/23.
//

#include "database_server.h"


namespace vanity {

DatabaseServer::DatabaseServer(std::optional<std::filesystem::path> db_file) noexcept
	: m_database{}
	, m_db_file{std::move(db_file)}
{
	if(m_db_file and std::filesystem::exists(m_db_file.value())){
		std::ifstream in{m_db_file.value(), std::ios::binary};
		m_database = db::Database::from(in);
		in.close();
	}
}

bool DatabaseServer::persist() const {
	if (!m_db_file)
		return false;

	auto tmp {m_db_file.value()};
	tmp.replace_filename("tmp." + tmp.filename().string());
	std::ofstream out{tmp, std::ios::binary};
	m_database.persist(out);
	out.close();
	std::filesystem::rename(tmp, m_db_file.value());

	return true;
}

void DatabaseServer::instruction_get(const ClientSocket &socket, const std::string &key) {
	if (!m_database.has(key))
		return send(socket, server_constants::null);

	auto value = m_database.get(key);
	switch (value.index()) {
		case 0:
			return send(socket, InstructionServer::prepare(std::get<0>(value)));
		case 1:
			return send(socket, InstructionServer::prepare(std::get<1>(value)));
		case 2:
			return send(socket, InstructionServer::prepare(std::get<2>(value)));
		default:
			throw std::runtime_error("invalid type");
	}
}

void DatabaseServer::instruction_set(const ClientSocket &socket, const std::string &key, const std::string &value) {
	m_database.set(key, value);
	send(socket, server_constants::ok);
}

void DatabaseServer::instruction_set(const ClientSocket &socket, const std::string &key, const int64_t &value) {
	m_database.set(key, value);
	send(socket, server_constants::ok);
}

void DatabaseServer::instruction_set(const ClientSocket &socket, const std::string &key, const double &value) {
	m_database.set(key, value);
	send(socket, server_constants::ok);
}

void DatabaseServer::instruction_del(const ClientSocket &socket, const std::string &key) {
	if (m_database.del(key))
		send(socket, server_constants::ok);
	else
		send(socket, server_constants::error);
}

void DatabaseServer::instruction_persist(const ClientSocket & socket) {
	if (persist()){
		send(socket, server_constants::ok);
	}
	else{
		static const std::string msg = std::string{server_constants::error} + ": Persistence disabled";
		send(socket, msg);
	}
}

void DatabaseServer::instruction_reset(const ClientSocket &socket) {
	m_database.reset();
	send(socket, server_constants::ok);
}

} // namespace vanity
