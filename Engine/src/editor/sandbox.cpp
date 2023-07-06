#include "sandbox.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <utility>

void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::CameraController*>(
        Yutrel::CameraController::Create((1920.0f / 1080.0f), glm::vec3(0.0f, 1.0f, 3.0f)));

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

void SpawnLight(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Light>(
        Yutrel::Light{glm::vec3(2.0f, 4.0f, 1.0f)});
}

void SpawnScene(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
    // model           = glm::scale(model, glm::vec3{0.1f, 0.1f, 0.1f});

    cmd.Spawn<Yutrel::Model*, Yutrel::Transform, Yutrel::Scene>(
        Yutrel::Model::Create("../resource/object/backpack/backpack.obj"),
        // Yutrel::Model::Create("../resource/object/nanosuit/nanosuit.obj"),
        // Yutrel::Model::Create("../resource/scene/cornell-box/cornell-box.obj"),
        // Yutrel::Model::Create("../resource/scene/staircase/stairscase.obj"),
        Yutrel::Transform{model},
        Yutrel::Scene{});

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3{10.0f, 10.0f, 10.0f});

    cmd.Spawn<Yutrel::Model*, Yutrel::Transform, Yutrel::Scene>(
        std::move(resources.Get<Yutrel::RenderData>().plane_model),
        Yutrel::Transform{model},
        Yutrel::Scene{});
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

    float delta_time = resources.Get<Yutrel::Time>().GetDeltaTime();
    camera_controller->Tick(delta_time);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    auto entity_skybox = querier.Query<Yutrel::SkyBox>();
    auto renderer      = resources.Get<Yutrel::Renderer*>();
    renderer->RenderScene(entity_camera_controller);
    renderer->RenderSkybox(entity_skybox[0], entity_camera_controller);
}