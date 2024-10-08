//
// Created by kingsli on 4/19/24.
//

#ifndef VANITY_LOGGER_H
#define VANITY_LOGGER_H

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <mutex>

#include "exceptions.h"


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

	// mutex for the logger
	std::mutex m_log_mutex;

	// the current log level
	LogLevel m_log_level;

public:
	// create a logger
	Logger(const std::filesystem::path& log_file, LogLevel log_level) {
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
	Logger(Logger&&)  noexcept = delete;
	Logger& operator=(Logger&&)  noexcept = delete;

	// set the log level
	// silently fails if logger is DISABLED
	void log_level(LogLevel log_level) {
		if (m_log_level != LogLevel::DISABLED) // can't re-enable logging
			m_log_level = log_level;
	}

	// get the log level
	LogLevel log_level() const{
		return m_log_level;
	}

	// flush the logger
	void flush() {
		std::lock_guard lock(m_log_mutex);
		m_log_file.flush();
	}

	// log a message
	void log(const std::string& msg, LogLevel log_level) {
		if (log_level < m_log_level or m_log_level == LogLevel::DISABLED)
			return;

		using std::chrono::system_clock;
		auto now = system_clock::to_time_t(system_clock::now());
		auto time = std::put_time(std::localtime(&now), "%T | ");

		std::lock_guard lock(m_log_mutex);
		m_log_file
			<< time
			<< log_level_str(log_level)
			<< ": "
			<< msg
			<< std::endl;
	}

	// log a debug message
	void debug(const std::string& msg) {
		log(msg, LogLevel::DEBUG);
	}

	// log a info message
	void info(const std::string& msg) {
		log(msg, LogLevel::INFO);
	}

	// log a warning message
	void warning(const std::string& msg) {
		log(msg, LogLevel::WARNING);
	}

	// log a error message
	void error(const std::string& msg) {
		log(msg, LogLevel::ERROR);
	}

	// log a critical message
	void critical(const std::string& msg) {
		log(msg, LogLevel::CRITICAL);
	}

	Logger& operator<<(const std::string& msg) {
		log(msg, LogLevel::INFO);
		return *this;
	}

	// get the string representation of the log level
	static const char* log_level_str(LogLevel log_level) {
		switch (log_level) {
			case LogLevel::DEBUG:
				return "DEBUG";
			case LogLevel::INFO:
				return "INFO";
			case LogLevel::WARNING:
				return "WARNING";
			case LogLevel::ERROR:
				return "ERROR";
			case LogLevel::CRITICAL:
				return "CRITICAL";
			case LogLevel::DISABLED:
				return "DISABLED";
			default:
				throw LogError("invalid log level");
		}
	}
};

} // namespace vanity

#endif //VANITY_LOGGER_H
