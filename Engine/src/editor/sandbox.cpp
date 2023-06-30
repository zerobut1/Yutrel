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

struct BackPack
{};

void SpawnBackpack(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Model*, BackPack>(
        Yutrel::Model::Create("../resource/object/backpack/backpack.obj"),
        BackPack{});
}

void SpawnTextureShader(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Shader*, BackPack>(
        Yutrel::Shader::Create("../Engine/asset/shader/shadow.vert", "../Engine/asset/shader/shadow.frag"),
        BackPack{});
}

// todo put into renderer
struct SkyBox
{};

void SpawnSkybox(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Model*, Yutrel::Shader*, Yutrel::TextureCubemaps*, SkyBox>(
        Yutrel::Model::Create("../resource/object/cube/cube.obj"),
        Yutrel::Shader::Create("../Engine/asset/shader/skybox.vert", "../Engine/asset/shader/skybox.frag"),
        Yutrel::TextureCubemaps::Create("../resource/texture/hdr/cayley_interior.hdr"),
        SkyBox{});
};

void DrawScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
{
    auto entities          = querier.Query<Yutrel::CameraController*>();
    auto camera_controller = querier.Get<Yutrel::CameraController*>(entities[0]);

    entities = querier.Query<Yutrel::Shader*>();
    Yutrel::Shader* backpack_shader;
    Yutrel::Shader* skybox_shader;
    for (auto& entity : entities)
    {
        if (querier.Has<BackPack>(entity))
        {
            backpack_shader = querier.Get<Yutrel::Shader*>(entity);
        }
        else if (querier.Has<SkyBox>(entity))
        {
            skybox_shader = querier.Get<Yutrel::Shader*>(entity);
        }
    }

    entities = querier.Query<Yutrel::Model*>();
    Yutrel::Model* backpack_model;
    Yutrel::Model* skybox_model;
    for (auto& entity : entities)
    {
        if (querier.Has<BackPack>(entity))
        {
            backpack_model = querier.Get<Yutrel::Model*>(entity);
        }
        else if (querier.Has<SkyBox>(entity))
        {
            skybox_model = querier.Get<Yutrel::Model*>(entity);
        }
    }

    entities = querier.Query<Yutrel::TextureCubemaps*>();
    Yutrel::TextureCubemaps* skybox_texture;
    for (auto& entity : entities)
    {
        if (querier.Has<SkyBox>(entity))
        {
            skybox_texture = querier.Get<Yutrel::TextureCubemaps*>(entity);
        }
    }

    float delta_time = resources.Get<Yutrel::Time>().GetDeltaTime();
    camera_controller->Tick(delta_time);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 model      = glm::mat4(1.0f);
    glm::mat4 view       = camera_controller->GetCamera().getViewMatrix();
    glm::mat4 projection = camera_controller->GetCamera().getProjectionMatrix();

    backpack_shader->Use();
    backpack_shader->setMat4("model", model);
    backpack_shader->setMat4("view", view);
    backpack_shader->setMat4("projection", projection);
    backpack_model->Draw();

    glDepthFunc(GL_LEQUAL);
    skybox_shader->Use();
    view = glm::mat4(glm::mat3(camera_controller->GetCamera().getViewMatrix()));
    skybox_shader->setMat4("view", view);
    skybox_shader->setMat4("projection", projection);
    skybox_texture->Bind();
    skybox_model->Draw();
    glDepthFunc(GL_LESS);

    // framebuffer->Unbind();
}