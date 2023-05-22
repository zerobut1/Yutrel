#include "yutrel_pch.h"

#include "global_context.h"

#include "runtime/platform/OpenGL/opengl_render_system.h"
#include "runtime/platform/Windows/windows_window_system.h"

namespace Yutrel
{
    RuntimeGlobalContext g_runtime_global_context;

    void RuntimeGlobalContext::startSystems()
    {
        // log system
        m_log_system = std::make_shared<LogSystem>();
        LOG_INFO("log system initialized!");

        // window system
        m_window_system = WindowSystem::create();
        WindowCreateInfo window_create_info;
        window_create_info.width  = 1920;
        window_create_info.height = 1080;
        window_create_info.title  = "Yutrel";
        m_window_system->initialize(window_create_info);
        LOG_INFO("window system initialized!");

        // input system
        m_input_system = InputSystem::create();
        m_input_system->initialize();
        LOG_INFO("input system initialized!");

        // render system
        m_render_system = RenderSystem::create();
        RenderSystemInitInfo render_init_info;
        render_init_info.window_system = m_window_system;
        m_render_system->initialize(render_init_info);
        LOG_INFO("render system initialized!");
    }

    void RuntimeGlobalContext::shutdownSystems()
    {
        m_render_system->clear();
        m_render_system.reset();

        m_input_system.reset();

        m_window_system.reset();

        m_log_system.reset();
    }
} // namespace Yutrel