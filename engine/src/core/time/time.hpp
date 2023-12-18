/**
 * 计时器类
 * 默认资源的一部分
 * 与平台有关
 * 提供计时，delta_time，fps等
 */
#pragma once

#include "core/macro.hpp"

namespace Yutrel
{
    class Time
    {
    public:
        static Ref<Time> Create();
        // static void Update(Commands& cmd, Querier querier, Resources resources, Events& events);

    public:
        virtual ~Time() = default;

        virtual void Tick() = 0;

        virtual float GetTime()      = 0;
        virtual float GetDeltaTime() = 0;
        virtual float GetFPS()       = 0;
        virtual float GetMS()        = 0;
    };
} // namespace Yutrel