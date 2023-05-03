#include "engine.h"

#include "runtime/function/global/global_context.h"
namespace Yutrel
{

    void YutrelEngine::startEngine()
    {
        // todo 还有反射系统，但是目前没有看懂
        g_runtime_global_context.startSystems();

        LOG_INFO("engine start!");
    }

    void YutrelEngine::shutdownEngine()
    {
        LOG_INFO("engine shutdown");

        g_runtime_global_context.shutdownSystems();
        // todo 反射系统注销
    }

    bool YutrelEngine::tickOneFrame()
    {
        // todo logicaltick
        // todo pollevents

        rendererTick();

        g_runtime_global_context.m_window_system->pollEvents();

        const bool should_window_close = g_runtime_global_context.m_window_system->shouldClose();

        return !should_window_close;
    }

    void YutrelEngine::rendererTick()
    {
        g_runtime_global_context.m_render_system->tick();
    }

} // namespace Yutrel