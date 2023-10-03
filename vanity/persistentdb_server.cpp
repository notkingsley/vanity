//
// Created by kingsli on 10/3/23.
//

#include "persistentdb_server.h"

namespace vanity{

PersistentDBServer::PersistentDBServer(std::optional<std::filesystem::path> db_file) noexcept
	: m_db_file{std::move(db_file)}
{
	if(m_db_file and std::filesystem::exists(m_db_file.value())){
		std::ifstream in{m_db_file.value(), std::ios::binary};
		m_database = db::Database::from(in);
		in.close();
		logger().info("Loaded database from " + m_db_file.value().string());
	}
}

bool PersistentDBServer::persist() const {
	if (!m_db_file)
		return false;

	auto tmp {m_db_file.value()};
	tmp.replace_filename("tmp." + tmp.filename().string());
	std::ofstream out{tmp, std::ios::binary};
	m_database.persist(out);
	out.close();
	std::filesystem::rename(tmp, m_db_file.value());

	logger().info("Persisted database to " + m_db_file.value().string());
	return true;
}

void PersistentDBServer::instruction_persist(const ClientSocket & socket) {
	if (persist()){
		send(socket, server_constants::ok);
	}
	else{
		static const std::string msg = std::string{server_constants::error} + ": Persistence disabled";
		send(socket, msg);
	}
}

} // namespace vanity