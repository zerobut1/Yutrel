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

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace Yutrel
