#pragma once

#include "Yutrel/core/log/log.hpp"

#define LOG_TRACE(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->trace("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_INFO(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->info("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_WARN(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->warn("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_ERROR(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->error("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_CRITICAL(...) Yutrel::Application::Get().GetWorld().GetResource<Log>()->GetLogger()->critical("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)

#define YUTREL_ASSERT(x, ...)                                \
    {                                                        \
        if (!(x))                                            \
        {                                                    \
            LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            __debugbreak();                                  \
        }                                                    \
    }