//
// Created by kingsli on 9/25/23.
//

#ifndef VANITY_LOGGING_H
#define VANITY_LOGGING_H

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
class logger_base
{
private:
	// the log file
	std::ofstream m_log_file;

	// the current log level
	LogLevel m_log_level;

public:
	// create a logger
	logger_base(const std::filesystem::path& log_file, LogLevel log_level){
		m_log_level = log_level;
		if (m_log_level != LogLevel::DISABLED)
			m_log_file.open(log_file, std::ios::out | std::ios::app);
	}

	// destroy the logger
	~logger_base() = default;

	// no copy
	logger_base(const logger_base&) = delete;
	logger_base& operator=(const logger_base&) = delete;

	// move
	logger_base(logger_base&&)  noexcept = default;
	logger_base& operator=(logger_base&&)  noexcept = default;

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
		static const char* log_level_str[] = {
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR",
			"CRITICAL",
		};

		if (log_level >= m_log_level and m_log_level != LogLevel::DISABLED){
			auto now = system_clock::to_time_t(system_clock::now());
			m_log_file
				<< std::put_time(std::localtime(&now), "%T | ")
				<< log_level_str[static_cast<int>(log_level)]
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

	logger_base& operator<<(const std::string& msg){
		log(msg, LogLevel::INFO);
		return *this;
	}
};

/*
 * A Logger allows us to log messages
 */
class Logger
{
private:
	// the logger
	mutable logger_base m_logger;

public:
	// create a logger
	explicit Logger(const std::filesystem::path& log_file, LogLevel log_level)
		: m_logger{log_file, log_level} {}

	// destroy the log server
	~Logger() = default;

	// no copy
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	// move
	Logger(Logger&&)  noexcept = default;
	Logger& operator=(Logger&&)  noexcept = default;

	// get the underlying logger
	logger_base & logger() const {
		return m_logger;
	}
};

} // namespace vanity

#endif //VANITY_LOGGING_H
