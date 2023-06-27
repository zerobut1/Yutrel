#include "yutrel_pch.h"

#include "application.hpp"

#include "Yutrel/core/log/log.hpp"
#include "Yutrel/function/window/window.hpp"
#include <winuser.h>

namespace Yutrel
{

    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        s_instance = this;

        m_world.SetResource(ExitTrigger{})
            .SetResource(Log{})
            .SetResource(Window::Create("Yutrel", 1024, 720))
            .AddSystem(UpdateWindow)
            .AddSystem(ExitTrigger::DetectShouldExit);
    }

    void Application::Init()
    {
        m_world.Startup();
        m_world.Update();

        LOG_INFO("Initlizing!");
    }

    void Application::Run()
    {
        Resources resources(m_world);
        auto& exit = resources.Get<ExitTrigger>();

        LOG_INFO("Start Running!");

        while (m_running)
        {
            m_world.Update();

            m_running = !exit.ShouldExit();
        }
    }

    void Application::Shutdown()
    {
        LOG_INFO("Shutdown!");

        m_world.Shutdown();
    }

    void ExitTrigger::DetectShouldExit(Commands& cmd,
                                       Querier querier,
                                       Resources resources,
                                       Events& events)
    {
        auto& trigger = resources.Get<ExitTrigger>();
        auto window   = resources.Get<Window*>();
        if (window->shouldClose())
        {
            trigger.Exit();
        }
    }

} // namespace Yutrel

// #include "yutrel_pch.h"

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