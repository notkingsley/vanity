//
// Created by kingsli on 1/14/24.
//

#include "config.h"

namespace vanity {

static inline void to_lower(std::string& str)
{
	std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			[](unsigned char c){ return std::tolower(c); }
	);
}

Config::Config(const Arguments &args) {
	auto root_dir = extract_root_dir(args);
	extract_ports(args);
	extract_db_file(args, root_dir);
	extract_users_db(args, root_dir);
	extract_logging(args, root_dir);
}

auto Config::extract_root_dir(const Arguments& args) -> path {
	auto root_dir = std::filesystem::current_path();
	if (not args.has("use_cwd")){
		root_dir = getpwuid(getuid())->pw_dir;
		root_dir /= DEFAULT_HOME_DIR;
		create_directories(root_dir);
	}
	return root_dir;
}

void Config::extract_ports(const Arguments& args) {
	if (args.has_multi_kwarg("port"))
		for (const auto& port: args.get_multi_kwarg("port"))
			ports.push_back(std::stoi(port));

	else if (args.has_kwarg("port"))
		ports.push_back(std::stoi(args.get_kwarg("port")));

	else
		for (const auto& port: DEFAULT_PORTS)
			ports.push_back(port);
}

void Config::extract_db_file(const Arguments& args, const path& root_dir) {
	if (args.has("no_db_persist")) {
		db_file = std::nullopt;
		return;
	}

	if (not args.has_kwarg("persist_file")){
		db_file = root_dir / DEFAULT_DB_FILE;
		return;
	}

	db_file = args.get_kwarg("persist_file");
	if (db_file->is_relative())
		db_file = root_dir / *db_file;
}

void Config::extract_users_db(const Arguments& args, const path& root_dir) {
	if (args.has("no_users_persist")) {
		users_db = std::nullopt;
		return;
	}

	if (not args.has_kwarg("users_file")){
		users_db = root_dir / DEFAULT_USERS_DB;
		return;
	}

	users_db = args.get_kwarg("users_file");
	if (users_db->is_relative())
		users_db = root_dir / *users_db;
}

void Config::_extract_log_file(const Arguments& args, const path& root_dir) {
	if (not args.has_kwarg("log_file")){
		log_file = root_dir / DEFAULT_LOG_FILE;
		return;
	}

	log_file = args.get_kwarg("log_file");
	if (log_file.is_relative())
		log_file = root_dir / log_file;
}

void Config::_extract_log_level(const Arguments& args) {
	if (not args.has_kwarg("log_level")){
		log_level = DEFAULT_LOG_LEVEL;
		return;
	}

	auto level = args.get_kwarg("log_level");
		to_lower(level);

	if (level == "debug")
		log_level = LogLevel::DEBUG;
	else if (level == "info")
		log_level = LogLevel::INFO;
	else if (level == "warning")
		log_level = LogLevel::WARNING;
	else if (level == "error")
		log_level = LogLevel::ERROR;
	else if (level == "critical")
		log_level = LogLevel::CRITICAL;
	else
		log_level = LogLevel::DISABLED;
}

void Config::extract_logging(const Arguments& args, const path& root_dir) {
	if (args.has("no_logging")) {
		log_level = LogLevel::DISABLED;
		log_file = path{};
		return;
	}
	_extract_log_file(args, root_dir);
	_extract_log_level(args);
}

} // namespace vanity