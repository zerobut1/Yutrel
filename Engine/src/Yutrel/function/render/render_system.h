#pragma once

#include "Yutrel/function/ui/window_ui.h"
#include "Yutrel/function/window/window_system.h"

#include <memory>

namespace Yutrel
{
    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    struct EngineContentViewport
    {
        float x      = 0.0f;
        float y      = 0.0f;
        float width  = 0.0f;
        float height = 0.0f;
    };

    // 最主要的渲染系统
    class RenderSystem
    {
    public:
        static std::shared_ptr<RenderSystem> create();

        virtual void initialize(RenderSystemInitInfo render_init_info) = 0;
        virtual void clear()                                           = 0;
        virtual void tick(float delta_time)                            = 0;

        // 初始化ui
        virtual void initializeUIRenderBackend(WindowUI *window_ui) = 0;

        // 显示渲染内容的窗口的viewport
        virtual void setEngineContentViewport(float offset_x, float offset_y, float width, float height) = 0;
        virtual EngineContentViewport getEngineContentViewport() const                                   = 0;

        // 临时
        virtual int getTexColorBuffer() const = 0;
    };
} // namespace Yutrel