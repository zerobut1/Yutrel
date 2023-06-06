#pragma once

#include <array>
#include <memory>
#include <stdint.h>

namespace Yutrel
{
    class WindowSystem;
    class RenderSystem;

    struct WindowUIInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
        std::shared_ptr<RenderSystem> render_system;
    };

    class WindowUI
    {
    public:
        virtual void initialize(WindowUIInitInfo init_info) = 0;
        virtual void preRender()                            = 0;
        virtual void setEngineOutputTextureID(uint32_t id)  = 0;
        virtual std::array<int, 2> getViewport()            = 0;
    };
} // namespace Yutrel