#include "yutrel_pch.hpp"

#include "renderer.hpp"

#include "Yutrel/platform/OpenGL/opengl_renderer.hpp"

namespace Yutrel
{
    Renderer* Renderer::Create(Window* window)
    {
        // 暂时直接使用opengl
        return new OpenGLRenderer(window);
    }

    void Renderer::RenderSkybox(Entity skybox, Entity camera_controller)
    {
        auto querier = Querier(Application::Get().GetWorld());
        auto data    = Application::Get().GetWorld().GetResource<RenderData>();
        auto texture = querier.Get<SkyBox>(skybox).texture;
        auto shader  = data->skybox_shader;
        auto model   = data->skybox_model;
        auto camera  = querier.Get<CameraController*>(camera_controller)->GetCamera();

        // todo in opengl renderer
        glDepthFunc(GL_LEQUAL);
        shader->Use();
        shader->setMat4("view", camera.getViewMatrix());
        shader->setMat4("projection", camera.getProjectionMatrix());
        texture->Bind();
        model->Draw();
        glDepthFunc(GL_LESS);
    }

} // namespace Yutrel