#pragma once

namespace Yutrel
{
    class Time
    {
    public:
        static float GetTime();
        static float GetDeltaTime();

    private:
        static float m_last_frametime;
    };
} // namespace Yutrel