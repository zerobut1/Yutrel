/**
 * Log类
 * 提供在控制台显示的Log信息
 * 不属于资源
 */
#pragma once

#include "core/macro.hpp"

#include <spdlog/spdlog.h>

namespace Yutrel
{
    class Log final
    {
    public:
        static Log& GetInstance();

    public:
        Log(Log& log)              = delete;
        void operator=(const Log&) = delete;

        Ref<spdlog::logger>& GetLogger() { return m_logger; }

    private:
        Log();
        ~Log();

    private:
        Ref<spdlog::logger> m_logger;
    };
} // namespace Yutrel

#define LOG_TRACE(...) Yutrel::Log::GetInstance().GetLogger()->trace("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_INFO(...) Yutrel::Log::GetInstance().GetLogger()->info("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_DEBUG(...) Yutrel::Log::GetInstance().GetLogger()->debug("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_WARN(...) Yutrel::Log::GetInstance().GetLogger()->warn("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_ERROR(...) Yutrel::Log::GetInstance().GetLogger()->error("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_CRITICAL(...) Yutrel::Log::GetInstance().GetLogger()->critical("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)

#define YUTREL_ASSERT(x, ...)          \
    {                                  \
        if (!(x))                      \
        {                              \
            LOG_CRITICAL(__VA_ARGS__); \
            __debugbreak();            \
        }                              \
    }