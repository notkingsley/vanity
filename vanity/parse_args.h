//
// Created by kingsli on 12/5/23.
//

#ifndef VANITY_PARSE_ARGS_H
#define VANITY_PARSE_ARGS_H

#include <pwd.h>
#include <unistd.h>

#include "arguments.h"
#include "server_config.h"


namespace vanity {

// extract a server config from the command line arguments
inline vanity::ServerConfig extract_config(const vanity::Arguments& args)
{
	vanity::ServerConfig config{};


	auto root_dir = std::filesystem::current_path();
	if (not args.has("use_cwd")){
		root_dir = getpwuid(getuid())->pw_dir;
		root_dir /= ".vanity";
		std::filesystem::create_directories(root_dir);
	}


	if (args.has_kwarg("port"))
		config.port = std::stoi(args.get_kwarg("port"));
	else
		config.port = 9955;


	if (args.has("no_db_persist")) {
		config.db_file = std::nullopt;
	}
	else {
		if (args.has_kwarg("persist_file")){
			std::filesystem::path db_file = args.get_kwarg("persist_file");
			if (db_file.is_relative())
				config.db_file = root_dir / db_file;
			else
				config.db_file = db_file;
		}
		else
			config.db_file = root_dir / "vanity.db";

	}


	if (args.has("no_users_persist")) {
		config.users_db = std::nullopt;
	}
	else {
		if (args.has_kwarg("users_file")){
			std::filesystem::path users_file = args.get_kwarg("users_file");
			if (users_file.is_relative())
				config.users_db = root_dir / users_file;
			else
				config.users_db = users_file;
		}
		else
			config.users_db = root_dir / "users.db";
	}


	if (args.has("no_logging")) {
		config.log_file = std::filesystem::path{};
		config.log_level = vanity::LogLevel::DISABLED;
	}
	else {
		if (args.has_kwarg("log_file")){
			std::filesystem::path log_file = args.get_kwarg("log_file");
			if (log_file.is_relative())
				config.log_file = root_dir / log_file;
			else
				config.log_file = log_file;
		}
		else
			config.log_file = root_dir / "vanity.log";

		if (args.has_kwarg("log_level")){
			auto level = args.get_kwarg("log_level");
			std::transform(
					level.begin(),
					level.end(),
					level.begin(),
					[](unsigned char c){ return std::tolower(c); }
			);
			if (level == "debug")
				config.log_level = vanity::LogLevel::DEBUG;
			else if (level == "info")
				config.log_level = vanity::LogLevel::INFO;
			else if (level == "warning")
				config.log_level = vanity::LogLevel::WARNING;
			else if (level == "error")
				config.log_level = vanity::LogLevel::ERROR;
			else if (level == "critical")
				config.log_level = vanity::LogLevel::CRITICAL;
			else
				config.log_level = vanity::LogLevel::DISABLED;
		}
		else
			config.log_level = vanity::LogLevel::INFO;
	}


	return config;
}

} // namespace vanity

#endif //VANITY_PARSE_ARGS_H
