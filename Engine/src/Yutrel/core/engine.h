#pragma once

#include "Yutrel/core/log/log_system.h"
#include "Yutrel/function/global/global_context.h"
#include "Yutrel/function/input/input_system.h"
#include "Yutrel/function/render/render_system.h"
#include "Yutrel/function/window/window_system.h"

namespace Yutrel
{
    class YutrelEngine
    {
        static const float s_fps_alpha;

    public:
        YutrelEngine()          = default;
        virtual ~YutrelEngine() = default;

        void startEngine();
        void shutdownEngine();

        float calculateDeltaTime();
        void calculateFPS(float delta_time);
        int getFPS() const { return m_fps; }

        std::shared_ptr<LogSystem> getLogSystem() { return g_runtime_global_context.m_log_system; };
        std::shared_ptr<WindowSystem> getWindowSystem() { return g_runtime_global_context.m_window_system; };
        std::shared_ptr<InputSystem> getInputSystem() { return g_runtime_global_context.m_input_system; };
        std::shared_ptr<RenderSystem> getRenderSystem() { return g_runtime_global_context.m_render_system; };

    private:
        void rendererTick(float delta_time);

    private:
        float m_lastframetime    = 0.0f;
        float m_average_duration = 0.0f;
        int m_frame_count        = 0;
        int m_fps                = 0;
    };
} // namespace Yutrel