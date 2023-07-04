#include "sandbox.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::CameraController*>(
        Yutrel::CameraController::Create((1920.0f / 1080.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    // todo set direction
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

struct BackPack
{};

void SpawnBackpack(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Model*, BackPack>(
        // Yutrel::Model::Create("../resource/object/backpack/backpack.obj"),
        // Yutrel::Model::Create("../resource/object/nanosuit/nanosuit.obj"),
        Yutrel::Model::Create("../resource/scene/cornell-box/cornell-box.obj"),
        // Yutrel::Model::Create("../resource/scene/staircase/stairscase.obj"),
        BackPack{});
}

void SpawnTextureShader(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Shader*, BackPack>(
        Yutrel::Shader::Create("../Engine/asset/shader/shadow.vert", "../Engine/asset/shader/shadow.frag"),
        BackPack{});
}

void SpawnSkybox(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::SkyBox>(
        Yutrel::SkyBox{
            Yutrel::TextureCubemaps::Create("../resource/texture/hdr/evening_road_01_puresky_4k.hdr")});
};

void DrawScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
{
    auto entity_camera_controller = querier.Query<Yutrel::CameraController*>()[0];
    auto camera_controller        = querier.Get<Yutrel::CameraController*>(entity_camera_controller);

    auto entities = querier.Query<Yutrel::Shader*>();
    Yutrel::Shader* backpack_shader;
    for (auto& entity : entities)
    {
        if (querier.Has<BackPack>(entity))
        {
            backpack_shader = querier.Get<Yutrel::Shader*>(entity);
        }
    }

    entities = querier.Query<Yutrel::Model*>();
    Yutrel::Model* backpack_model;
    for (auto& entity : entities)
    {
        if (querier.Has<BackPack>(entity))
        {
            backpack_model = querier.Get<Yutrel::Model*>(entity);
        }
    }

    float delta_time = resources.Get<Yutrel::Time>().GetDeltaTime();
    camera_controller->Tick(delta_time);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 model      = glm::mat4(1.0f);
    model                = glm::scale(model, glm::vec3{0.1f, 0.1f, 0.1f});
    glm::mat4 view       = camera_controller->GetCamera().getViewMatrix();
    glm::mat4 projection = camera_controller->GetCamera().getProjectionMatrix();

    backpack_shader->Use();
    backpack_shader->setMat4("model", model);
    backpack_shader->setMat4("view", view);
    backpack_shader->setMat4("projection", projection);
    backpack_model->Draw(backpack_shader);

    auto entity_skybox = querier.Query<Yutrel::SkyBox>();
    auto renderer      = resources.Get<Yutrel::Renderer*>();
    renderer->RenderSkybox(entity_skybox[0], entity_camera_controller);

    // framebuffer->Unbind();
}