#pragma once

#include "Yutrel/core/application/application.hpp"

#include <iostream>
#include <spdlog/spdlog.h>

#include <memory>

namespace Yutrel
{
    class Log final
    {
    public:
        Log();
        virtual ~Log();

        inline std::shared_ptr<spdlog::logger> GetLogger()
        {
            return m_logger;
        }

        void test()
        {
            std::cout << "test log" << std::endl;
        }

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace Yutrel

#define LOG_TRACE(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->trace("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_INFO(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->info("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_WARN(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->warn("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_ERROR(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->error("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_CRITICAL(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->critical("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)