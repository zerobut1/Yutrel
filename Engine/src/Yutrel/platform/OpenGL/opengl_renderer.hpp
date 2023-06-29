#pragma once

#include "Yutrel/function/render/camera_controller.hpp"
#include "Yutrel/function/render/renderer.hpp"
#include "Yutrel/platform/OpenGL/opengl_model.hpp"
#include "Yutrel/platform/OpenGL/opengl_shader.hpp"
#include "Yutrel/platform/OpenGL/opengl_texture.hpp"
#include "Yutrel/platform/OpenGL/opengl_vertex_array.h"

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
        virtual void Tick(float delta_time) override{};

        // ui
        virtual void InitializeUIRender(WindowUI* window_ui) override;
        virtual void RenderUI(std::shared_ptr<WindowUI> ui) override;
        virtual void ClearUIRender(WindowUI* window_ui) override;
    };

} // namespace Yutrel