#include <iostream>
#include <pwd.h>

#include "arguments.h"
#include "server.h"


// extract a server config from the command line arguments
static inline vanity::ServerConfig extract_config(const vanity::Arguments& args)
{
	vanity::ServerConfig config{};


	if (args.has_kwarg("port"))
		config.port = std::stoi(args.get_kwarg("port"));
	else
		config.port = 9955;


	if (args.has_kwarg("persist_file") and args.has("no_persist")){
		std::cout << "Error: --no-persist and --persist-file are mutually exclusive\n";
		exit(1);
	}
	else if (args.has("no_persist")){
		config.db_file = std::nullopt;
	}
	else if (args.has_kwarg("persist_file")){
		config.db_file = args.get_kwarg("persist_file");
		if (not config.db_file->is_absolute())
			std::cout << "Warning: persist file is not absolute" << std::endl;
	}
	else{
		if (args.has("persist_to_cwd"))
			config.db_file = std::filesystem::current_path();
		else
			config.db_file = getpwuid(getuid())->pw_dir;
		config.db_file.value() /= "vanity.db";
	}


	if (args.has_kwarg("log_file"))
		config.log_file = args.get_kwarg("log_file");
	else
		config.log_file = std::filesystem::current_path() / "vanity.log";

	if (args.has_kwarg("log_level")){
		auto level = args.get_kwarg("log_level");
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
		else{
			std::cout << "Error: invalid log level" << std::endl;
			exit(1);
		}
	}

	if (args.has("disable_logging"))
		config.log_level = vanity::LogLevel::DISABLED;


	return config;
}

int main(int argc, char **argv)
{
	vanity::Arguments args{argc, argv};

	vanity::Server server{extract_config(args)};
	server.run();

	return 0;
}