#pragma once

#include "Yutrel/function/render/camera_controller.hpp"
#include "Yutrel/function/render/component.hpp"
#include "Yutrel/function/render/framebuffer.hpp"
#include "Yutrel/function/render/model.hpp"
#include "Yutrel/function/render/shader.hpp"
#include "Yutrel/function/ui/window_ui.hpp"
#include "Yutrel/function/window/window.hpp"
#include "glm/fwd.hpp"

#include <memory>

namespace Yutrel
{
    struct RenderData
    {
        Model* cube_model;
        Model* plane_model;

        Shader* skybox_shader;
    };

    struct Transform
    {
        glm::mat4 model_matrix;
    };

    // 目前只是简单的定向光
    struct Light
    {
        glm::vec3 light_pos;
    };

    struct Scene
    {};

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
        virtual void RenderScene(Entity shader_entity, Entity camera_controller_entity) = 0;
        virtual void RenderSkybox(Entity skybox, Entity camera_controller_entity)       = 0;

        virtual void DrawIndexed(const VertexArray* vertexArray) = 0;

        // ui
        virtual void InitializeUIRender(WindowUI* window_ui) = 0;
        virtual void RenderUI(WindowUI* ui)                  = 0;
        virtual void ClearUIRender(WindowUI* window_ui)      = 0;
    };
} // namespace Yutrel