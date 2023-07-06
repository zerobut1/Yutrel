#pragma once

#include "Yutrel/function/render/camera_controller.hpp"
#include "Yutrel/function/render/renderer.hpp"
#include "Yutrel/platform/OpenGL/opengl_framebuffer.hpp"
#include "Yutrel/platform/OpenGL/opengl_shader.hpp"
#include "Yutrel/platform/OpenGL/opengl_texture.hpp"
#include "Yutrel/platform/OpenGL/opengl_vertex_array.hpp"

#include "GLFW/glfw3.h"

#include <memory>
#include <vector>

namespace Yutrel
{
    /*
     * OpenGL 渲染器的实现
     */
    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer(Window* window);
        virtual ~OpenGLRenderer();

        virtual void Initialize(Window* window) override;

        // 现在是空函数，后面也许会有用
        virtual void Clear() override{};

        virtual void RenderScene(Entity shader_entity, Entity camera_controller_entity) override;
        virtual void RenderSkybox(Entity skybox, Entity camera_controller_entity) override;

        virtual void DrawIndexed(const VertexArray* vertexArray) override;

        // ui
        virtual void InitializeUIRender(WindowUI* window_ui) override;
        virtual void RenderUI(WindowUI* ui) override;
        virtual void ClearUIRender(WindowUI* window_ui) override;
    };

} // namespace Yutrel