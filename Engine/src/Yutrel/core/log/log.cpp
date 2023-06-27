#include "yutrel_pch.h"

#include "log.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Yutrel
{
    Log::Log()
    {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        logSinks[0]->set_pattern("[%T] [%^%l%$] %v");

        m_logger = std::make_shared<spdlog::logger>("Yutrel", begin(logSinks), end(logSinks));
        spdlog::register_logger(m_logger);
        m_logger->set_level(spdlog::level::trace);
        m_logger->flush_on(spdlog::level::trace);
    }

    Log::~Log()
    {
        m_logger->flush();
        spdlog::drop_all();
    }
} // namespace Yutrel