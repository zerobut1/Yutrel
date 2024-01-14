#pragma once

#include <chrono>
namespace Yutrel
{
    class Time final
    {
    public:
        Time();
        Time(const Time&)            = delete;
        Time& operator=(const Time&) = delete;
        Time(Time&&)                 = default;
        Time& operator=(Time&&)      = default;

        uint32_t GetFPS() const { return m_fps; }

        // 返回以微秒为单位的时间
        uint32_t Elapse() const;

        static void Update(gecs::resource<gecs::mut<Time>> time);

    private:
        std::chrono::steady_clock::time_point m_cur_time;
        std::chrono::steady_clock::duration m_elapse;
        uint32_t m_fps;
    };
} // namespace Yutrel