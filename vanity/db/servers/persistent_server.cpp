//
// Created by kingsli on 10/3/23.
//

#include "persistent_server.h"

namespace vanity{

PersistentServer::PersistentServer(std::optional<std::filesystem::path> db_file) noexcept
	: m_db_file{std::move(db_file)}
{
	if(m_db_file and std::filesystem::exists(m_db_file.value())){
		std::ifstream in{m_db_file.value(), std::ios::binary};
		for (auto& db : m_databases)
			db = db::Database::from(in);
		in.close();

		logger().info("Loaded databases from " + m_db_file.value().string());
	}
}

bool PersistentServer::persist() {
	if (!m_db_file)
		return false;
	{
		auto tmp{m_db_file.value()};
		tmp.replace_filename("tmp." + tmp.filename().string());
		{
			std::ofstream out{tmp, std::ios::binary};
			for (auto& db : m_databases)
				db.persist(out);
			out.close();
		}
		std::filesystem::rename(tmp, m_db_file.value());
	}
	logger().info("Persisted databases to " + m_db_file.value().string());
	return true;
}

void PersistentServer::request_persist(Client & client) {
	if (persist())
		send(client, ok());
	else
		send(client, error("Persistence disabled"));
}

void PersistentServer::start() {
	if (not m_db_file)
		return;

	m_stopped.clear();
	m_persist_loop_thread = std::thread(&PersistentServer::persist_loop, this);
}

void PersistentServer::stop() {
	if (not m_db_file)
		return;

	persist();
	m_stopped.set();
	m_persist_loop_thread.join();
}

void PersistentServer::persist_loop() {
	while (not m_stopped.wait(M_PERSIST_INTERVAL))
		push_event(server_event::persist);
}

void PersistentServer::event_persist() {
	persist();
}

} // namespace vanity