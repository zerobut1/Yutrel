#include "yutrel_pch.hpp"

#include "Yutrel/core/time.hpp"

#include "GLFW/glfw3.h"

namespace Yutrel
{
    float Time::m_last_frametime = 0.0f;

    float Time::GetTime()
    {
        return glfwGetTime();
    }

    float Time::GetDeltaTime()
    {
        float delta_time;

        float time       = Time::GetTime();
        delta_time       = time - m_last_frametime;
        m_last_frametime = time;

        return delta_time;
    }
} // namespace Yutrel