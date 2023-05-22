#include "yutrel_pch.cpp"

#include "engine.h"

#include "runtime/core/time.h"
#include "runtime/function/global/global_context.h"

namespace Yutrel
{
    void YutrelEngine::startEngine()
    {
        // todo 反射系统，但是目前没有看懂
        g_runtime_global_context.startSystems();

        LOG_INFO("engine start!");
    }

    void YutrelEngine::shutdownEngine()
    {
        LOG_INFO("engine shutdown");

        g_runtime_global_context.shutdownSystems();
        // todo 反射系统注销
    }

    /**
     * 逻辑tick(暂未实现)
     * 交换数据(暂未实现)
     * 渲染tick
     */
    bool YutrelEngine::tickOneFrame(float delta_time)
    {
        calculateFPS(delta_time);

        // todo logicaltick

        // todo swapdata

        rendererTick(delta_time);

        g_runtime_global_context.m_window_system->pollEvents();

        g_runtime_global_context.m_window_system->setTitle(
            std::string("Yutrel - " + std::to_string(getFPS()) + " FPS").c_str());

        const bool should_window_close = g_runtime_global_context.m_window_system->shouldClose();
        return !should_window_close;
    }

    void YutrelEngine::rendererTick(float delta_time)
    {
        g_runtime_global_context.m_render_system->tick(delta_time);
    }

    float YutrelEngine::calculateDeltaTime()
    {
        float delta_time;

        float time      = Time::getTime();
        delta_time      = time - m_lastframetime;
        m_lastframetime = time;

        return delta_time;
    }

    const float YutrelEngine::s_fps_alpha = 1.0f / 100.0f;
    void YutrelEngine::calculateFPS(float delta_time)
    {
        // 平均采样法计算fps
        // 这个方法计算出来的fps有点奇怪
        m_frame_count++;

        if (m_frame_count == 1)
        {
            m_average_duration = delta_time;
        }
        else
        {
            m_average_duration = m_average_duration * (1 - s_fps_alpha) + delta_time * s_fps_alpha;
        }

        m_fps = static_cast<int>(1.f / m_average_duration);
    }

} // namespace Yutrel