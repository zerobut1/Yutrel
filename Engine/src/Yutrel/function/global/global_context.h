#pragma once

#include "Yutrel/core/log/log_system.h"
#include "Yutrel/function/input/input_system.h"
#include "Yutrel/function/render/render_system.h"
#include "Yutrel/function/window/window_system.h"

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
        std::shared_ptr<InputSystem> m_input_system;
        std::shared_ptr<RenderSystem> m_render_system;
    };

    extern RuntimeGlobalContext g_runtime_global_context;
} // namespace Yutrel