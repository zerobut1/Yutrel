#pragma once

#include "runtime/core/log_system.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/window/window_system.h"

#include <memory>

namespace Yutrel
{

    class RuntimeGlobalContext
    {
    public:
        void startSystems();

        void shutdownSystems();

    public:
        std::shared_ptr<LogSystem> m_log_system;
        std::shared_ptr<WindowSystem> m_window_system;
        std::shared_ptr<RenderSystem> m_render_system;
    };

    extern RuntimeGlobalContext g_runtime_global_context;
} // namespace Yutrel