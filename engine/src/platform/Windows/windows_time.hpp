#pragma once

#include "core/time/time.hpp"

namespace Yutrel
{
    class WindowsTime : public Time
    {
    public:
        WindowsTime()          = default;
        virtual ~WindowsTime() = default;

        virtual void Tick() override;

        virtual float GetTime() override { return m_time; }
        virtual float GetDeltaTime() override { return m_delta_time; }
        virtual float GetFPS() override { return m_fps; }
        virtual float GetMS() override { return m_ms; }

    private:
        float m_time = 0.0f;

        float m_this_frametime = 0.0f;
        float m_last_frametime = 0.0f;
        float m_delta_time     = 0.0f;

        float m_fps       = 0.0f;
        float m_ms        = 0.0f;
        float m_duration  = 0.0f;
        int m_frame_count = 0;
    };
} // namespace Yutrel