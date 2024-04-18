//
// Created by kingsli on 4/18/24.
//

#ifndef VANITY_GLOBAL_LOG_H
#define VANITY_GLOBAL_LOG_H

#include "log_server.h"

/*
 * This file defines a global logger
 *
 * This exists for the sole purpose of
 * testing and debugging. It is not
 * intended for actual use.
 */

namespace vanity {

constexpr bool GLOBAL_LOGGING = true;

// the global logger
Logger global_logger("global.log", LogLevel::DEBUG);

// log something to the global logger
void global_log(const std::string& msg) {
	if constexpr (GLOBAL_LOGGING)
		global_logger.debug(msg);
}

} // namespace vanity

#endif //VANITY_GLOBAL_LOG_H
