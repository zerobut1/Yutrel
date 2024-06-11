#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace Yutrel
{
    class Log final
    {
    public:
        static Log& Get();

    public:
        Log(Log& log)              = delete;
        void operator=(const Log&) = delete;

        std::shared_ptr<spdlog::logger>& GetLogger() { return m_logger; }

    private:
        Log();
        ~Log();

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace Yutrel

#define LOG_TRACE(...) Yutrel::Log::Get().GetLogger()->trace("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_INFO(...) Yutrel::Log::Get().GetLogger()->info("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_DEBUG(...) Yutrel::Log::Get().GetLogger()->debug("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_WARN(...) Yutrel::Log::Get().GetLogger()->warn("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_ERROR(...) Yutrel::Log::Get().GetLogger()->error("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_CRITICAL(...) Yutrel::Log::Get().GetLogger()->critical("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)

#define YUTREL_ASSERT(x, ...)          \
    {                                  \
        if (!(x))                      \
        {                              \
            LOG_CRITICAL(__VA_ARGS__); \
            __debugbreak();            \
        }                              \
    }