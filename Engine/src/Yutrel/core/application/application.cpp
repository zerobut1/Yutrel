#include "yutrel_pch.hpp"

#include "application.hpp"

#include "Yutrel/core/log/log.hpp"
#include "Yutrel/function/input/input.hpp"
#include "Yutrel/function/render/renderer.hpp"
#include "Yutrel/function/window/window.hpp"

namespace Yutrel
{
    Application* CreateApplication()
    {
        return new Application;
    }

    // 用Get()获取
    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        s_instance = this;

        m_world
            .SetResource(ExitTrigger{})
            .SetResource(Log{})
            .SetResource(Window::Create("Yutrel", 1920, 1080))
            .SetResource(Input::Create())
            .SetResource(Renderer::Create(*m_world.GetResource<Window*>()))
            .AddSystem(UpdateWindow)
            .AddSystem(ExitTrigger::DetectShouldExit);
    }

    void Application::Init()
    {
        LOG_INFO("Initlizing!");

        m_world.Startup();
        m_world.Update();
    }

    void Application::Run()
    {
        LOG_INFO("Start Running!");

        Resources resources(m_world);
        auto& exit = resources.Get<ExitTrigger>();

        // 主循环
        while (!exit.ShouldExit())
        {
            m_world.Update();
        }
    }

    void Application::Shutdown()
    {
        LOG_INFO("Shutdown!");

        m_world.Shutdown();
    }

    void ExitTrigger::DetectShouldExit(Commands& cmd, Querier querier, Resources resources, Events& events)
    {
        // 目前是直接检测window是否shouldclose
        auto& trigger = resources.Get<ExitTrigger>();
        auto window   = resources.Get<Window*>();
        if (window->ShouldClose())
        {
            trigger.Exit();
        }
    }

} // namespace Yutrel

// #include "yutrel_pch.hpp"

// #include "application.h"

// namespace Yutrel
// {

//     Application::Application(YutrelEngine *engine)
//     {
//         m_engine   = engine;
//     }

//     void Application::run()
//     {
//         float delta_time;
//         while (m_running)
//         {
//             delta_time = m_engine->calculateDeltaTime();
//             m_engine->calculateFPS(delta_time);

//             tick(delta_time);
//             // m_engine->getRenderSystem()->tick(delta_time);

//             m_engine->getRenderSystem()->renderUI(m_ui);

//             m_engine->getWindowSystem()->tick();

//             m_engine->getWindowSystem()->setTitle(
//                 std::string("Yutrel-Editor - " + std::to_string(m_engine->getFPS()) + " FPS").c_str());

//             m_running = !g_runtime_global_context.m_window_system->shouldClose();
//         }
//     }
// } // namespace Yutrel