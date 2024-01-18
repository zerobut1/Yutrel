#include "yutrel_pch.hpp"

#include "time.hpp"
#include <stdint.h>

namespace Yutrel
{
    Time::Time()
        : m_elapse(1)
    {
        m_cur_time = std::chrono::steady_clock::now();
    }

    uint32_t Time::Elapse() const
    {
        auto elapse = std::chrono::duration_cast<std::chrono::microseconds>(m_elapse).count();
        return elapse > 0 ? elapse : 1;
    }

    void Time::Update(gecs::resource<gecs::mut<Time>> time)
    {
        time->m_elapse = std::chrono::steady_clock::now() - time->m_cur_time;

        time->m_cur_time = std::chrono::steady_clock::now();

        static uint32_t frame_count = 0;
        static uint32_t duration    = 0;

        frame_count++;
        duration += time->Elapse();
        // 0.5秒更新一次
        if (duration >= 500000)
        {
            time->m_fps = (frame_count * 1000000) / duration;
            frame_count = 0;
            duration    = 0;
        }
    }

} // namespace Yutrel