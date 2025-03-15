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
        Log(const Log&)            = delete;
        Log& operator=(const Log&) = delete;

        std::shared_ptr<spdlog::logger>& getLogger() { return m_logger; }

    private:
        Log();
        ~Log();

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace Yutrel

#define LOG_TRACE(...) Yutrel::Log::Get().getLogger()->trace("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_INFO(...) Yutrel::Log::Get().getLogger()->info("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_DEBUG(...) Yutrel::Log::Get().getLogger()->debug("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_WARN(...) Yutrel::Log::Get().getLogger()->warn("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_ERROR(...) Yutrel::Log::Get().getLogger()->error("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_CRITICAL(...) Yutrel::Log::Get().getLogger()->critical("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)

#define YUTREL_ASSERT(x, ...)          \
    {                                  \
        if (!(x))                      \
        {                              \
            LOG_CRITICAL(__VA_ARGS__); \
            __debugbreak();            \
        }                              \
    }