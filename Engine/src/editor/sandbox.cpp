#include "sandbox.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_clip_space.hpp"
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

void SpawnLight(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    cmd.Spawn<Yutrel::Light>(
        Yutrel::Light{glm::vec3(2.0f, 4.0f, 1.0f)});
}

void SpawnScene(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    // shader
    cmd.Spawn<Yutrel::Shader*, Yutrel::Scene>(
        Yutrel::Shader::Create("../Engine/asset/shader/shadow.vert", "../Engine/asset/shader/shadow.frag"),
        Yutrel::Scene{});

    glm::mat4 model = glm::mat4(1.0f);

    // 模型
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
    // model           = glm::scale(model, glm::vec3{0.1f, 0.1f, 0.1f});
    cmd.Spawn<Yutrel::Model*, Yutrel::Transform, Yutrel::Scene>(
        Yutrel::Model::Create("../resource/object/backpack/backpack.obj"),
        // Yutrel::Model::Create("../resource/object/nanosuit/nanosuit.obj"),
        // Yutrel::Model::Create("../resource/scene/cornell-box/cornell-box.obj"),
        // Yutrel::Model::Create("../resource/scene/staircase/stairscase.obj"),
        Yutrel::Transform{model},
        Yutrel::Scene{});

    // 平面
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

struct Shadowmap
{};

void SpawnShadowmap(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    Yutrel::FramebufferSpecification fbSpec;
    fbSpec.Attachments = {Yutrel::FramebufferTextureFormat::DEPTH32};
    fbSpec.Width       = 1024;
    fbSpec.Height      = 1024;

    cmd.Spawn<Yutrel::Framebuffer*, Yutrel::Shader*, Shadowmap>(
        Yutrel::Framebuffer::Create(fbSpec),
        Yutrel::Shader::Create("../Engine/asset/shader/shadowmap.vert", "../Engine/asset/shader/shadowmap.frag"),
        Shadowmap{});
}

void UpdateScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
{
    // 获取摄像机
    auto camera_controller_entity = querier.Query<Yutrel::CameraController*>()[0];
    auto camera_controller        = querier.Get<Yutrel::CameraController*>(camera_controller_entity);

    // 获取 framebuffer
    auto viewport_eneity      = querier.Query<Viewport>()[0];
    auto viewport_framebuffer = querier.Get<Yutrel::Framebuffer*>(viewport_eneity);
    auto viewport             = querier.Get<Viewport>(viewport_eneity);

    // viewport有变化则resize
    if (Yutrel::FramebufferSpecification spec = viewport_framebuffer->getSpecification();
        viewport.width > 0.0f && viewport.height > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != viewport.width || spec.Height != viewport.height))
    {
        viewport_framebuffer->resize((uint32_t)viewport.width, (uint32_t)viewport.height);
        // todo : camera resize
        camera_controller->Resize((float)viewport.width / (float)viewport.height);
    }

    // camera
    // if (m_viewport_focused)
    {
        // 更新摄像机
        float delta_time = resources.Get<Yutrel::Time>().GetDeltaTime();
        camera_controller->Tick(delta_time);
    }
}

void DrawScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
{

    // 获取摄像机
    auto camera_controller_entity = querier.Query<Yutrel::CameraController*>()[0];
    auto camera_controller        = querier.Get<Yutrel::CameraController*>(camera_controller_entity);

    // 获取renderer
    auto renderer = resources.Get<Yutrel::Renderer*>();

    //-----------------------------
    // 渲染shadowmap
    //-----------------------------
    auto shadowmap_entity      = querier.Query<Shadowmap>()[0];
    auto shadowmap_framebuffer = querier.Get<Yutrel::Framebuffer*>(shadowmap_entity);
    auto shadowmap_shader      = querier.Get<Yutrel::Shader*>(shadowmap_entity);

    shadowmap_framebuffer->Bind();
    // todo in renderer
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowmap_shader->Use();
    auto light_entities          = querier.Query<Yutrel::Light>();
    auto directional_light       = querier.Get<Yutrel::Light>(light_entities[0]);
    glm::mat4 light_view         = glm::lookAt(directional_light.light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_projection   = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);
    glm::mat4 light_space_matrix = light_projection * light_view;
    shadowmap_shader->setMat4("lightSpaceMatrix", light_space_matrix);
    renderer->RenderScene(shadowmap_entity, camera_controller_entity);
    shadowmap_framebuffer->Unbind();

    //-----------------------------
    // 渲染scene
    //-----------------------------
    auto viewport_eneity      = querier.Query<Viewport>()[0];
    auto viewport_framebuffer = querier.Get<Yutrel::Framebuffer*>(viewport_eneity);
    viewport_framebuffer->Bind();

    // todo in Renderer
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    auto entity_skybox           = querier.Query<Yutrel::SkyBox>();
    auto entities                = querier.Query<Yutrel::Scene>();
    Yutrel::Entity shader_entity = NULL;
    for (auto entity : entities)
    {
        if (querier.Has<Yutrel::Shader*>(entity))
        {
            shader_entity = entity;
            break;
        }
    }
    auto shader = querier.Get<Yutrel::Shader*>(shader_entity);
    shader->Use();
    shader->setMat4("lightSpaceMatrix", light_space_matrix);
    shader->setFloat3("viewPos", camera_controller->GetCamera().getPosition());
    shader->setFloat3("lightPos", directional_light.light_pos);
    glBindTextureUnit(4, shadowmap_framebuffer->getColorAttachmentRendererID());

    renderer->RenderScene(shader_entity, camera_controller_entity);
    renderer->RenderSkybox(entity_skybox[0], camera_controller_entity);

    viewport_framebuffer->Unbind();
}

void SpawnViewport(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    Viewport viewport;
    viewport.width  = 1920;
    viewport.height = 1080;

    Yutrel::FramebufferSpecification fbSpec;
    fbSpec.Attachments = {Yutrel::FramebufferTextureFormat::RGBA8,
                          Yutrel::FramebufferTextureFormat::DEPTH24STENCIL8};
    fbSpec.Width       = viewport.width;
    fbSpec.Height      = viewport.height;

    cmd.Spawn<Yutrel::Framebuffer*, Viewport>(
        Yutrel::Framebuffer::Create(fbSpec),
        Viewport{viewport});
}

void SpawnUI(Yutrel::Commands& cmd, Yutrel::Resources resources)
{
    auto ui = new EditorUI();
    cmd.Spawn<EditorUI*>(
        std::move(ui));
}
void DrawUI(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
{
    auto ui_entity = querier.Query<EditorUI*>()[0];
    auto ui        = querier.Get<EditorUI*>(ui_entity);
    auto renderer  = resources.Get<Yutrel::Renderer*>();
    renderer->RenderUI(ui);
}