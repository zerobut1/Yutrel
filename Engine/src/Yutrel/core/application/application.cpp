#include "yutrel_pch.h"

#include "application.h"

namespace Yutrel
{
    Application::Application(YutrelEngine *engine)
    {
        m_engine_runtime = engine;
    }

    void Application::run()
    {
        float delta_time;
        while (true)
        {
            delta_time = m_engine_runtime->calculateDeltaTime();
            tick();
            if (!m_engine_runtime->tickOneFrame(delta_time))
            {
                return;
            }
        }
    }
} // namespace Yutrel