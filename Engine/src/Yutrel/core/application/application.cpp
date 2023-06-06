#include "yutrel_pch.h"

#include "application.h"

namespace Yutrel
{

    Application::Application(YutrelEngine *engine)
    {
        m_engine   = engine;
    }

    void Application::run()
    {
        float delta_time;
        while (m_running)
        {
            delta_time = m_engine->calculateDeltaTime();
            m_engine->calculateFPS(delta_time);

            tick(delta_time);
            // m_engine->getRenderSystem()->tick(delta_time);

            m_engine->getRenderSystem()->renderUI(m_ui);

            m_engine->getWindowSystem()->tick();

            m_engine->getWindowSystem()->setTitle(
                std::string("Yutrel-Editor - " + std::to_string(m_engine->getFPS()) + " FPS").c_str());

            m_running = !g_runtime_global_context.m_window_system->shouldClose();
        }
    }
} // namespace Yutrel