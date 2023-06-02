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
        virtual void clear() override;

        virtual void tick(float delta_time) override;

        virtual void initializeUIRenderBackend(WindowUI *window_ui) override;
        virtual void setEngineContentViewport(float offset_x, float offset_y, float width, float height) override;
        virtual EngineContentViewport getEngineContentViewport() const override;

        virtual int getTexColorBuffer() const override { return m_renderer_output->getRnedererID(); }

    private:
        void refreshOutputFrameBuffer();
        // void refreshShadowMapFrameBuffer();

        void drawScene(std::shared_ptr<Shader> &shader);

    private:
        GLFWwindow *m_window;
        WindowUI *m_ui;
        EngineContentViewport m_viewport;

        std::shared_ptr<CameraController> m_camera_controller;

        // skybox
        std::shared_ptr<VertexArray> m_skybox_VAO;
        std::shared_ptr<Shader> m_skybox_shader;
        std::shared_ptr<TextureCubemaps> m_skybox_texture;

        // plane
        std::shared_ptr<VertexArray> m_plane_VAO;
        std::shared_ptr<Shader> m_plane_shader;
        std::shared_ptr<Texture2D> m_plane_texture;

        // bunny
        std::shared_ptr<Model> m_bunny_model;
        std::shared_ptr<Shader> m_bunny_shader;

        // plane
        // std::shared_ptr<VertexArray> m_quad_VAO;
        // std::shared_ptr<Shader> m_quad_shader;
        // std::shared_ptr<Texture2D> m_quad_texture;

        //--------------tmp------------
        unsigned int m_output_renderbuffer, m_output_framebuffer;
        std::shared_ptr<Texture2D> m_renderer_output;

        unsigned int m_shadowmap_framebuffer , m_shadowmap_texture;
        // std::shared_ptr<Texture2D> m_shadowmap_texture;
        std::shared_ptr<Shader> m_shadowmap_shader;
    };

} // namespace Yutrel