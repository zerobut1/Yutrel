#include "imgui_ui.hpp"

#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <memory>
#include <stdint.h>

void ImguiUI::RenderUI()
{
    if (ImGui::Begin("Status"))
    {
        ImGui::Text("FPS: %d", fps);

        ImGui::InputFloat3("平行光方向", reinterpret_cast<float*>(&directional_light_direction));

        ImGui::End();
    }

    // ImGui::ShowDemoWindow();
}

void ImguiUI::Init()
{
    directional_light_direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.25f));
}

void ImguiUI::UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                         gecs::querier<gecs::mut<Yutrel::DirectionalLight>> directional_lights,
                         gecs::resource<Yutrel::Time> time)
{
    auto imgui_ui = std::reinterpret_pointer_cast<ImguiUI>(ui->ui);

    for (auto&& [_, light] : directional_lights)
    {
        light.direction = glm::normalize(imgui_ui->directional_light_direction);
    }

    imgui_ui->fps = time->GetFPS();
}