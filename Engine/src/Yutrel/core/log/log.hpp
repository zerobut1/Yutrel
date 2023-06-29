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
