#include "yutrel_pch.hpp"

#include "windows_time.hpp"

#include "GLFW/glfw3.h"

namespace Yutrel
{
    void WindowsTime::Tick()
    {
        m_this_frametime = static_cast<float>(glfwGetTime());
        m_delta_time     = m_this_frametime - m_last_frametime;
        m_last_frametime = m_this_frametime;

        m_time = m_this_frametime;

        m_frame_count++;
        m_duration += m_delta_time;

        if (m_duration >= 0.1f)
        {
            m_fps         = m_frame_count * 1.0f / m_duration;
            m_ms          = 1000.0f * m_duration / m_frame_count * 1.0f;
            m_frame_count = 0.0f;
            m_duration    = 0.0f;
        }
    }
} // namespace Yutrel