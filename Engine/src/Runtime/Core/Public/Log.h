#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace Yutrel
{
    class Log final
    {
    public:
        enum class LogLevel : uint8_t
        {
            Debug,
            Info,
            Warn,
            Error,
            Fatal
        };

        static Log& Get();

    public:
        Log(const Log&)            = delete;
        Log& operator=(const Log&) = delete;

        template <typename... Args>
        void log(LogLevel level, Args&&... args)
        {
            switch (level)
            {
            case LogLevel::Debug:
                m_logger->debug(std::forward<Args>(args)...);
                break;
            case LogLevel::Info:
                m_logger->info(std::forward<Args>(args)...);
                break;
            case LogLevel::Warn:
                m_logger->warn(std::forward<Args>(args)...);
                break;
            case LogLevel::Error:
                m_logger->error(std::forward<Args>(args)...);
                break;
            case LogLevel::Fatal:
                m_logger->critical(std::forward<Args>(args)...);
                break;
            default:
                break;
            }
        }

    private:
        Log();
        ~Log();

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace Yutrel

#define LOG_DEBUG(...) Yutrel::Log::Get().log(Yutrel::Log::LogLevel::Debug, "[" + std::string(__FUNCTION__) + "] " + std::format(__VA_ARGS__))
#define LOG_INFO(...) Yutrel::Log::Get().log(Yutrel::Log::LogLevel::Info, "[" + std::string(__FUNCTION__) + "] " + std::format(__VA_ARGS__))
#define LOG_WARN(...) Yutrel::Log::Get().log(Yutrel::Log::LogLevel::Warn, "[" + std::string(__FUNCTION__) + "] " + std::format(__VA_ARGS__))
#define LOG_ERROR(...) Yutrel::Log::Get().log(Yutrel::Log::LogLevel::Error, "[" + std::string(__FUNCTION__) + "] " + std::format(__VA_ARGS__))
#define LOG_FATAL(...) Yutrel::Log::Get().log(Yutrel::Log::LogLevel::Fatal, "[" + std::string(__FUNCTION__) + "] " + std::format(__VA_ARGS__))

#define YUTREL_ASSERT(x, ...)       \
    {                               \
        if (!(x))                   \
        {                           \
            LOG_FATAL(__VA_ARGS__); \
            __debugbreak();         \
        }                           \
    }