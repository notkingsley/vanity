//
// Created by kingsli on 12/5/23.
//

#ifndef VANITY_SERVER_CONFIG_H
#define VANITY_SERVER_CONFIG_H

#include <filesystem>
#include <optional>
#include <vector>

#include "utils/logging.h"


namespace vanity {

/*
 * Configuration for the server
 */
struct ServerConfig
{
	std::optional<std::filesystem::path> users_db;
	std::optional<std::filesystem::path> db_file;
	std::filesystem::path log_file;
	std::vector<uint16_t> ports;
	LogLevel log_level;
};

} // namespace vanity

#endif //VANITY_SERVER_CONFIG_H
