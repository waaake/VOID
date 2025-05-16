/* Internal */
#include "Logging.h"

#ifdef VOID_ENABLE_LOGGING
/* SpdLog */
#include <spdlog/sinks/stdout_color_sinks.h>

VOID_NAMESPACE_OPEN

std::shared_ptr<spdlog::logger> Logger::logger_;

void Logger::Init()
{
    /* Create Consoles for each of the Logger */
    /* Setup multi-threaded sinks */
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    /* Init Logger */
    logger_ = std::make_shared<spdlog::logger>("VOID Logger", spdlog::sinks_init_list{console_sink});
    /* Setup logging level */
    logger_->set_level(spdlog::level::level_enum::info);
}

VOID_NAMESPACE_CLOSE
#endif
