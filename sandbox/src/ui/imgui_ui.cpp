#include "imgui_ui.hpp"

#include <glm/glm.hpp>
#include <imgui.h>
#include <memory>

void ImguiUI::RenderUI()
{
    if (ImGui::Begin("background"))
    {
        ImGui::InputFloat4("data1", (float*)&background.data1);
        ImGui::InputFloat4("data2", (float*)&background.data2);
        ImGui::InputFloat4("data3", (float*)&background.data3);
        ImGui::InputFloat4("data4", (float*)&background.data4);

        ImGui::End();
    }
}

void ImguiUI::UpdateData(gecs::resource<Yutrel::UIResource> ui,
                         gecs::resource<gecs::mut<Yutrel::BackGroundColor>> background_color)
{
    background_color.get() = std::reinterpret_pointer_cast<ImguiUI>(ui->ui)->background;
}