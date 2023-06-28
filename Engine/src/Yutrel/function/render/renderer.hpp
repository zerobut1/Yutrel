#pragma once

#include "Yutrel/function/ui/window_ui.h"
#include "Yutrel/function/window/window.hpp"

#include <memory>

namespace Yutrel
{
    /*
     * 渲染器
     */
    class Renderer
    {
    public:
        static Renderer* Create(Window* window);

        virtual void Initialize(Window* window) = 0;
        virtual void Clear()                    = 0;
        virtual void Tick(float delta_time)     = 0;

        // ui
        virtual void InitializeUIRender(WindowUI* window_ui) = 0;
        virtual void RenderUI(std::shared_ptr<WindowUI> ui)  = 0;
        virtual void ClearUIRender(WindowUI* window_ui)      = 0;
    };
} // namespace Yutrel