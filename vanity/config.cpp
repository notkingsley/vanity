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
	extract_working_dir(args);
	extract_ports(args);
	extract_logging(args);
	extract_db_file(args);
	extract_wal_file(args);
	extract_auth_file(args);
	extract_lock_file(args);
	extract_journal_file(args);
}

void Config::extract_working_dir(const Arguments& args) {
	if (args.has("no_working_dir"))
		return;

	using std::filesystem::absolute;
	if (args.has_kwarg("working_dir"))
		working_dir = absolute(args.get_kwarg("working_dir"));
	else
		working_dir = get_user_working_dir();

	create_directories(*working_dir);
}

auto Config::get_user_home_dir() -> path {
	return getpwuid(getuid())->pw_dir;
}

auto Config::get_user_working_dir() -> path {
	return get_user_home_dir() / DEFAULT_HOME_DIR;
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

LogLevel Config::extract_log_level(const Arguments& args) {
	if (not args.has_kwarg("log_level"))
		return DEFAULT_LOG_LEVEL;

	auto level = args.get_kwarg("log_level");
	to_lower(level);

	if (level == "debug")
		return LogLevel::DEBUG;
	else if (level == "info")
		return LogLevel::INFO;
	else if (level == "warning")
		return LogLevel::WARNING;
	else if (level == "error")
		return LogLevel::ERROR;
	else if (level == "critical")
		return LogLevel::CRITICAL;
	else
		return LogLevel::DISABLED;
}

void Config::extract_logging(const Arguments& args) {
	if (not working_dir or args.has("no_logging")) {
		log_level = LogLevel::DISABLED;
		return;
	}

	log_file = *working_dir / LOG_FILE;
	log_level = extract_log_level(args);
}

void Config::extract_db_file(const Arguments& args) {
	if (working_dir and not args.has("no_db_persist"))
		db_file = *working_dir / DB_FILE;
}

void Config::extract_auth_file(const Arguments& args) {
	if (working_dir and not args.has("no_auth_persist"))
		users_db = *working_dir / AUTH_FILE;
}

void Config::extract_wal_file(const Arguments &args) {
	if (working_dir and not args.has("no_wal"))
		wal_file = *working_dir / WAL_FILE;
}

void Config::extract_journal_file(const Arguments &) {
	if (working_dir)
		journal_file = *working_dir / JOURNAL_FILE;
}

void Config::extract_lock_file(const Arguments &args) {
	if (working_dir)
		lock_file = *working_dir / LOCK_FILE;
}

} // namespace vanity