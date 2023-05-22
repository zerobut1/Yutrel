#pragma once

#include "runtime/function/render/camera_controller.h"
#include "runtime/function/render/render_system.h"
#include "runtime/platform/OpenGL/opengl_model.h"
#include "runtime/platform/OpenGL/opengl_shader.h"
#include "runtime/platform/OpenGL/opengl_texture.h"
#include "runtime/platform/OpenGL/opengl_vertex_array.h"

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
        virtual void clear() override;

        virtual void tick(float delta_time) override;

        virtual void initializeUIRenderBackend(WindowUI *window_ui) override;
        virtual void setEngineContentViewport(float offset_x, float offset_y, float width, float height) override;
        virtual EngineContentViewport getEngineContentViewport() const override;

        virtual int getTexColorBuffer() const override { return texColorBuffer; }

    private:
        void refreshFrameBuffer();

    private:
        GLFWwindow *m_window;
        WindowUI *m_ui;
        EngineContentViewport m_viewport;

        std::shared_ptr<CameraController> m_test_camera_controller;

        // skybox
        std::shared_ptr<VertexArray> m_skybox_VAO;
        std::shared_ptr<Shader> m_skybox_shader;
        std::shared_ptr<TextureCubemaps> m_skybox_texture;

        std::shared_ptr<Shader> m_model_shader;
        std::shared_ptr<Texture2D> m_test_texture;
        std::shared_ptr<Model> m_test_model;

        //--------------tmp------------
        unsigned int texColorBuffer, texDepthBuffer, framebuffer = 0;
    };

} // namespace Yutrel