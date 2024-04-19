//
// Created by kingsli on 9/25/23.
//

#ifndef VANITY_LOG_SERVER_H
#define VANITY_LOG_SERVER_H

#include "utils/logger.h"

namespace vanity {

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
	LogServer(LogServer&&)  noexcept = delete;
	LogServer& operator=(LogServer&&)  noexcept = delete;

	// get the underlying logger
	Logger& logger() {
		return m_logger;
	}
};

} // namespace vanity

#endif //VANITY_LOG_SERVER_H
