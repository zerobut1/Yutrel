#pragma once

#include "runtime/function/window/window_system.h"

#include <memory>

namespace Yutrel
{
    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    class RenderSystem
    {
    public:
        RenderSystem()          = default;
        virtual ~RenderSystem() = default;

        virtual void initialize(RenderSystemInitInfo render_init_info) = 0;

        virtual void tick() = 0;
    };
} // namespace Yutrel