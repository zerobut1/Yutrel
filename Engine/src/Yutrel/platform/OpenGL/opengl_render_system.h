#pragma once

#include "Yutrel/function/render/camera_controller.h"
#include "Yutrel/function/render/render_system.h"
#include "Yutrel/platform/OpenGL/opengl_model.h"
#include "Yutrel/platform/OpenGL/opengl_shader.h"
#include "Yutrel/platform/OpenGL/opengl_texture.h"
#include "Yutrel/platform/OpenGL/opengl_vertex_array.h"

#include "GLFW/glfw3.h"
#include <memory>
#include <vector>

namespace Yutrel
{
    class OpenGLRenderSystem : public RenderSystem
    {
    public:
        OpenGLRenderSystem() = default;
        virtual ~OpenGLRenderSystem();

        virtual void initialize(RenderSystemInitInfo render_init_info) override;

        // 现在是空函数，后面也许会有用
        virtual void clear() override{};
        virtual void tick(float delta_time) override{};

        virtual void initializeUIRenderBackend(WindowUI *window_ui) override;
        virtual void renderUI(std::shared_ptr<WindowUI> ui) override;
    };

} // namespace Yutrel