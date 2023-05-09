#pragma once

#include "runtime/function/render/render_system.h"
#include "runtime/platform/OpenGL/opengl_shader.h"

#include "GLFW/glfw3.h"
#include <memory>

namespace Yutrel
{
    class OpenGL_RenderSystem : public RenderSystem
    {
    public:
        OpenGL_RenderSystem() = default;
        virtual ~OpenGL_RenderSystem();

        virtual void initialize(RenderSystemInitInfo render_init_info) override;
        virtual void clear() override;

        virtual void tick(float delta_time) override;

        virtual void initializeUIRenderBackend(WindowUI *window_ui) override;
        virtual void updateEngineContentViewport(float offset_x, float offset_y, float width, float height) override;
        virtual EngineContentViewport getEngineContentViewport() const override;

        virtual unsigned int getFrameBuffer() const override { return framebuffer; }
        virtual unsigned int getTexColorBuffer() const override { return texColorBuffer; }
        virtual unsigned int getTexDepthBuffer() const override { return texDepthBuffer; }

    private:
        void refreshFrameBuffer();

        GLFWwindow *m_window;
        WindowUI *m_ui;
        EngineContentViewport m_viewport;

        //--------tmp--------

        unsigned int VAO, VBO;
        //--------------
        unsigned int texColorBuffer, texDepthBuffer, framebuffer = 0;

        std::shared_ptr<Shader> m_test_shader;
    };
} // namespace Yutrel