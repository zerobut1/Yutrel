#include <Yutrel.hpp>

#include "ui/imgui_ui.hpp"

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
            {glm::mat4(1.0f)},
        });

    auto sun = cmds.create();
    Yutrel::DirectionalLight directional_light{};
    directional_light.direction = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f));
    cmds.emplace<Yutrel::DirectionalLight>(sun, std::move(directional_light));
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