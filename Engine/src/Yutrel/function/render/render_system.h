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

    // 最主要的渲染系统
    class RenderSystem
    {
    public:
        static std::shared_ptr<RenderSystem> Create();

        virtual void initialize(RenderSystemInitInfo render_init_info) = 0;
        virtual void clear()                                           = 0;
        virtual void tick(float delta_time)                            = 0;

        // virtual void DrawModel(std::shared_ptr<Model> model,)

        // ui
        virtual void initializeUIRender(WindowUI *window_ui) = 0;
        virtual void renderUI(std::shared_ptr<WindowUI> ui)  = 0;
        virtual void clearUIRender(WindowUI *window_ui)      = 0;
    };
} // namespace Yutrel