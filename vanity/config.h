//
// Created by kingsli on 1/14/24.
//

#ifndef VANITY_CONFIG_H
#define VANITY_CONFIG_H

#include <filesystem>
#include <optional>
#include <pwd.h>
#include <unistd.h>
#include <vector>

#include "arguments.h"
#include "log_server.h"

namespace vanity {

/*
 * Configuration for a server
 */
class Config
{
private:
	using path = std::filesystem::path;

	// extract the working directory
	// should be called before any other extraction
	void extract_working_dir(const Arguments& args);

	// get the user's home directory
	static path get_user_home_dir();

	// get the user global working directory
	static path get_user_working_dir();

	// extract the ports
	void extract_ports(const Arguments& args);

	// extract and returns the log level from the arguments
	static LogLevel extract_log_level(const Arguments& args);

	// extract the logging configuration
	void extract_logging(const Arguments& args);

	// extract the database file
	void extract_db_file(const Arguments& args);

	// extract the auth database file
	void extract_auth_file(const Arguments& args);

	// extract the wal file
	void extract_wal_file(const Arguments& args);

	// extract the journal file
	void extract_journal_file(const Arguments&);

	// extract the lock file
	void extract_lock_file(const Arguments& args);

	// extract the host
	void extract_host(const Arguments& args);

	// extract the cluster port
	void extract_cluster_port(const Arguments& args);

	// get the default cluster port
	// this pops the last port from the ports vector
	uint16_t get_default_cluster_port();

public:
	constexpr static auto DEFAULT_LOG_LEVEL = LogLevel::INFO;
	constexpr static auto DEFAULT_PORTS = {9955, 19955};
	constexpr static auto DEFAULT_HOST = "localhost";
	constexpr static auto DEFAULT_HOME_DIR = ".vanity";
	constexpr static auto DB_FILE = "vanity.db";
	constexpr static auto LOG_FILE = "vanity.log";
	constexpr static auto WAL_FILE = "vanity.wal";
	constexpr static auto AUTH_FILE = "vanity.auth";
	constexpr static auto LOCK_FILE = "vanity.lock";
	constexpr static auto JOURNAL_FILE = "vanity.journal";

	std::optional<path> journal_file;
	std::optional<path> working_dir;
	std::optional<path> lock_file;
	std::optional<path> auth_file;
	std::optional<path> wal_file;
	std::optional<path> db_file;
	path log_file;

	std::string host;
	uint16_t cluster_port;
	std::vector<uint16_t> ports;
	LogLevel log_level = DEFAULT_LOG_LEVEL;

	Config(const Arguments& args);
};

} // namespace vanity

#endif //VANITY_CONFIG_H
