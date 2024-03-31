#include <Yutrel.hpp>

#include "ui/imgui_ui.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include <utility>

void SetUp(gecs::commands cmds,
           gecs::resource<gecs::mut<Yutrel::AssetManager>> asset_manager)
{
    auto sponza = cmds.create();
    auto scene  = asset_manager->AddGLTFScene("resource/sponza/sponza.gltf");
    cmds.emplace_bundle<Yutrel::SceneBundle>(
        sponza,
        Yutrel::SceneBundle{
            scene,
        });

    auto sky = cmds.create();
    Yutrel::DirectionalLight directional_light{};
    directional_light.direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.25f));
    cmds.emplace<Yutrel::DirectionalLight>(sky, std::move(directional_light));
    cmds.emplace<Yutrel::Skybox>(
        sky,
        Yutrel::Skybox{
            asset_manager->AddHDRTexture("resource/skybox/brdf_lut.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_irradiance_X+.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_irradiance_X-.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_irradiance_Y+.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_irradiance_Y-.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_irradiance_Z+.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_irradiance_Z-.hdr"),

            asset_manager->AddHDRTexture("resource/skybox/skybox_specular_X+.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_specular_X-.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_specular_Y+.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_specular_Y-.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_specular_Z+.hdr"),
            asset_manager->AddHDRTexture("resource/skybox/skybox_specular_Z-.hdr"),

            asset_manager->AddMesh("resource/skybox/cube.obj"),
        });

    // auto water      = cmds.create();
    // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
    // model           = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
    // cmds.emplace<Yutrel::Water>(
    //     water,
    //     Yutrel::Water{
    //         asset_manager->AddMesh("resource/plane/plane.obj"),
    //         asset_manager->AddTexture("resource/texture/water.png"),
    //         {model},
    //     });
}

void UpdateCamera(gecs::resource<gecs::mut<Yutrel::Camera>> camera,
                  gecs::resource<Yutrel::Time> time,
                  gecs::resource<Yutrel::Input> input)
{
    if (input->IsKeyPressed(Yutrel::Key::W))
    {
        camera->MoveFront();
    }
    if (input->IsKeyPressed(Yutrel::Key::S))
    {
        camera->MoveBack();
    }
    if (input->IsKeyPressed(Yutrel::Key::A))
    {
        camera->MoveLeft();
    }
    if (input->IsKeyPressed(Yutrel::Key::D))
    {
        camera->MoveRight();
    }
    if (input->IsKeyPressed(Yutrel::Key::Space))
    {
        camera->MoveUp();
    }
    if (input->IsKeyPressed(Yutrel::Key::LeftShift))
    {
        camera->MoveDown();
    }

    static bool first_mouse = true;
    if (input->IsMousePressed(Yutrel::Mouse::ButtonRight))
    {
        static glm::vec2 last_pos;
        glm::vec2 cur_pos = input->GetCursorPos();
        if (first_mouse)
        {
            last_pos    = cur_pos;
            first_mouse = false;
        }
        camera->MoveDirection(cur_pos.x - last_pos.x, last_pos.y - cur_pos.y);
        last_pos = cur_pos;
    }
    else
    {
        first_mouse = true;
    }

    camera->Updata(time->GetDeltaTime());
}

int main()
{
    Yutrel::Application::Create()
        .Init("Sandbox", 1920, 1080)
        .AddResource<Yutrel::UIResource>(Yutrel::CreateRef<ImguiUI>())
        .AddResource<Yutrel::Camera>(glm::vec3{0.0f, 3.0f, 0.0f})
        .AddStartupSystem<SetUp>()
        .AddSystem<ImguiUI::UpdateData>()
        .AddSystem<UpdateCamera>()
        .Run();

    return 0;
}