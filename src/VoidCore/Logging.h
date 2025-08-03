// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_LOGGING_H
#define _VOID_LOGGING_H

#ifdef VOID_ENABLE_LOGGING
/* SpdLog */
#include <spdlog/spdlog.h>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API Logger
{
public:
    static void Init();

    /* Fetch the current logger instance */
    inline static std::shared_ptr<spdlog::logger> GetLogger() { return logger_; }

private:
    /* Speed Logger */
    static std::shared_ptr<spdlog::logger> logger_;
};

VOID_NAMESPACE_CLOSE

#define VOID_LOG_TRACE(...)         VOID_NAMESPACE::Logger::GetLogger()->trace(__VA_ARGS__)
#define VOID_LOG_INFO(...)          VOID_NAMESPACE::Logger::GetLogger()->info(__VA_ARGS__)
#define VOID_LOG_WARN(...)          VOID_NAMESPACE::Logger::GetLogger()->warn(__VA_ARGS__)
#define VOID_LOG_ERROR(...)         VOID_NAMESPACE::Logger::GetLogger()->error(__VA_ARGS__)
#define VOID_LOG_CRITICAL(...)      VOID_NAMESPACE::Logger::GetLogger()->critical(__VA_ARGS__)
#else
#define VOID_LOG_TRACE(...)
#define VOID_LOG_INFO(...)
#define VOID_LOG_WARN(...)
#define VOID_LOG_ERROR(...)
#define VOID_LOG_CRITICAL(...)
#endif

#endif // _VOID_LOGGING_H
