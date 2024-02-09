//
// Created by kingsli on 9/25/23.
//

#ifndef VANITY_LOG_SERVER_H
#define VANITY_LOG_SERVER_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>


namespace vanity {

enum class LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	CRITICAL,
	DISABLED,
};

/*
 * A simple logger class
 */
class Logger
{
private:
	// the log file
	std::ofstream m_log_file;

	// the current log level
	LogLevel m_log_level;

public:
	// create a logger
	Logger(const std::filesystem::path& log_file, LogLevel log_level){
		m_log_level = log_level;
		if (m_log_level != LogLevel::DISABLED)
			m_log_file.open(log_file, std::ios::out | std::ios::app);
	}

	// destroy the logger
	~Logger() = default;

	// no copy
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	// move
	Logger(Logger&&)  noexcept = default;
	Logger& operator=(Logger&&)  noexcept = default;

	// set the log level
	// silently fails if logger is DISABLED
	void log_level(LogLevel log_level){
		if (m_log_level != LogLevel::DISABLED) // can't re-enable logging
			m_log_level = log_level;
	}

	// get the log level
	LogLevel log_level() const{
		return m_log_level;
	}

	// flush the logger
	void flush(){
		m_log_file.flush();
	}

	// log a message
	void log(const std::string& msg, LogLevel log_level){
		using std::chrono::system_clock;

		const char* log_level_str = nullptr;
		switch (log_level) {
			case LogLevel::DEBUG:
				log_level_str = "DEBUG";
				break;
			case LogLevel::INFO:
				log_level_str = "INFO";
				break;
			case LogLevel::WARNING:
				log_level_str = "WARNING";
				break;
			case LogLevel::ERROR:
				log_level_str = "ERROR";
				break;
			case LogLevel::CRITICAL:
				log_level_str = "CRITICAL";
				break;
			case LogLevel::DISABLED:
				break;
		}

		if (log_level >= m_log_level and m_log_level != LogLevel::DISABLED){
			auto now = system_clock::to_time_t(system_clock::now());
			m_log_file
				<< std::put_time(std::localtime(&now), "%T | ")
				<< log_level_str
				<< ": "
				<< msg
				<< std::endl;
		}
	}

	// log a debug message
	void debug(const std::string& msg){
		log(msg, LogLevel::DEBUG);
	}

	// log a info message
	void info(const std::string& msg){
		log(msg, LogLevel::INFO);
	}

	// log a warning message
	void warning(const std::string& msg){
		log(msg, LogLevel::WARNING);
	}

	// log a error message
	void error(const std::string& msg){
		log(msg, LogLevel::ERROR);
	}

	// log a critical message
	void critical(const std::string& msg){
		log(msg, LogLevel::CRITICAL);
	}

	Logger& operator<<(const std::string& msg){
		log(msg, LogLevel::INFO);
		return *this;
	}
};

/*
 * A LogServer allows us to log messages
 */
class LogServer
{
private:
	// the logger
	Logger m_logger;

public:
	// create a LogServer
	explicit LogServer(const std::filesystem::path& log_file, LogLevel log_level)
		: m_logger{log_file, log_level} {}

	// destroy the log server
	~LogServer() = default;

	// no copy
	LogServer(const LogServer&) = delete;
	LogServer& operator=(const LogServer&) = delete;

	// move
	LogServer(LogServer&&)  noexcept = default;
	LogServer& operator=(LogServer&&)  noexcept = default;

	// get the underlying logger
	Logger & logger() {
		return m_logger;
	}
};

} // namespace vanity

#endif //VANITY_LOG_SERVER_H
