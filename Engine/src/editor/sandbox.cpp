#include "sandbox.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::CameraController*>(
        Yutrel::CameraController::Create((1920.0f / 1080.0f), glm::vec3(0.0f, 1.0f, 4.0f)));
}

void SpawnFramebuffer(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    Yutrel::FramebufferSpecification fbSpec;
    fbSpec.Attachments = {Yutrel::FramebufferTextureFormat::RGBA8,
                          Yutrel::FramebufferTextureFormat::DEPTH24STENCIL8};
    fbSpec.Width       = 1920;
    fbSpec.Height      = 1080;

    cmd.Spawn<Yutrel::Framebuffer*>(
        Yutrel::Framebuffer::Create(fbSpec));
}

void SpawnBackpack(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Model*>(
        Yutrel::Model::Create("../resource/object/backpack/backpack.obj"));
}

void SpawnTextureShader(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Shader*>(
        Yutrel::Shader::Create("../Engine/asset/shader/shadow.vert", "../Engine/asset/shader/shadow.frag"));
}

void DrawScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
{
    auto entity            = querier.Query<Yutrel::CameraController*>();
    auto camera_controller = querier.Get<Yutrel::CameraController*>(entity[0]);
    // camera_controller->Tick(0.0f);

    entity           = querier.Query<Yutrel::Framebuffer*>();
    auto framebuffer = querier.Get<Yutrel::Framebuffer*>(entity[0]);

    entity      = querier.Query<Yutrel::Shader*>();
    auto shader = querier.Get<Yutrel::Shader*>(entity[0]);

    entity      = querier.Query<Yutrel::Model*>();
    auto model_ = querier.Get<Yutrel::Model*>(entity[0]);

    // framebuffer->Bind();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glEnable(GL_DEPTH_TEST);

    glm::mat4 model      = glm::mat4(1.0f);
    glm::mat4 view       = camera_controller->GetCamera().getViewMatrix();
    glm::mat4 projection = camera_controller->GetCamera().getProjectionMatrix();

    shader->Use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    model_->Draw();

    // framebuffer->Unbind();
}