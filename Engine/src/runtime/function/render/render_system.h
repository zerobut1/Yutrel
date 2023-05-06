#pragma once

#include "editor/editor_ui.h"
#include "runtime/function/ui/window_ui.h"
#include "runtime/function/window/window_system.h"

#include <memory>

namespace Yutrel
{
    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    struct EngineContentViewport
    {
        float x{0.0f};
        float y{0.0f};
        float width{0.0f};
        float height{0.0f};
    };

    class RenderSystem
    {
    public:
        RenderSystem()          = default;
        virtual ~RenderSystem() = default;

        virtual void initialize(RenderSystemInitInfo render_init_info) = 0;

        virtual void tick(float delta_time) = 0;

        virtual void initializeUIRenderBackend(WindowUI *window_ui)                                         = 0;
        virtual void updateEngineContentViewport(float offset_x, float offset_y, float width, float height) = 0;
        virtual EngineContentViewport getEngineContentViewport() const                                      = 0;

        virtual unsigned int getFrameBuffer() const    = 0;
        virtual unsigned int getTexColorBuffer() const = 0;
        virtual unsigned int getTexDepthBuffer() const = 0;
    };
} // namespace Yutrel