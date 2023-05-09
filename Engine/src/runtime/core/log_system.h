#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace Yutrel
{
    class LogSystem final
    {
    public:
        LogSystem();
        ~LogSystem();

        inline std::shared_ptr<spdlog::logger> &getLogger()
        {
            return m_logger;
        }

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace Yutrel

#define LOG_TRACE(...)      g_runtime_global_context.m_log_system->getLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)       g_runtime_global_context.m_log_system->getLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)       g_runtime_global_context.m_log_system->getLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)      g_runtime_global_context.m_log_system->getLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)   g_runtime_global_context.m_log_system->getLogger()->critical(__VA_ARGS__)