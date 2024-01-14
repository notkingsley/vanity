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
#include "utils/logging.h"

namespace vanity {

/*
 * Configuration for a server
 */
class Config
{
private:
	using path = std::filesystem::path;

	// extract the root directory
	static path extract_root_dir(const Arguments& args);

	// extract the ports
	void extract_ports(const Arguments& args);

	// extract the database file
	void extract_db_file(const Arguments& args, const path& root_dir);

	// extract the users database file
	void extract_users_db(const Arguments& args, const path& root_dir);

	// extract the log file
	// assumes no_logging is not set
	void _extract_log_file(const Arguments& args, const path& root_dir);

	// extract the log level
	// assumes no_logging is not set
	void _extract_log_level(const Arguments& args);

	// extract the logging configuration
	void extract_logging(const Arguments& args, const path& root_dir);

public:
	constexpr static auto DEFAULT_PORTS = std::array<uint16_t, 2>{9955, 19955};
	constexpr static auto DEFAULT_LOG_LEVEL = LogLevel::INFO;
	constexpr static auto DEFAULT_HOME_DIR = ".vanity";
	constexpr static auto DEFAULT_DB_FILE = "vanity.db";
	constexpr static auto DEFAULT_USERS_DB = "users.db";
	constexpr static auto DEFAULT_LOG_FILE = "vanity.log";

	std::optional<path> users_db;
	std::optional<path> db_file;
	path log_file;
	std::vector<uint16_t> ports;
	LogLevel log_level = DEFAULT_LOG_LEVEL;

	explicit Config(const Arguments& args);
};

} // namespace vanity

#endif //VANITY_CONFIG_H
