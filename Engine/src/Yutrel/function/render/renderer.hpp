#pragma once

#include "Yutrel/function/render/component.hpp"
#include "Yutrel/function/render/model.hpp"
#include "Yutrel/function/render/shader.hpp"
#include "Yutrel/function/ui/window_ui.h"
#include "Yutrel/function/window/window.hpp"

#include <memory>

namespace Yutrel
{
    struct RenderData
    {
        Model* skybox_model;
        Shader* skybox_shader;
    };

    /*
     * 渲染器
     */
    class Renderer
    {
    public:
        static Renderer* Create(Window* window);

        virtual void Initialize(Window* window) = 0;
        virtual void Clear()                    = 0;

        // virtual void RenderModel(const Model* model)                       = 0;
        virtual void RenderSkybox(Entity skybox, Entity camera_controller) = 0;

        virtual void DrawIndexed(const VertexArray* vertexArray) = 0;

        // ui
        // virtual void InitializeUIRender(WindowUI* window_ui) = 0;
        // virtual void RenderUI(std::shared_ptr<WindowUI> ui)  = 0;
        // virtual void ClearUIRender(WindowUI* window_ui)      = 0;
    };
} // namespace Yutrel