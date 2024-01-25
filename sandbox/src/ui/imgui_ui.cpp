#include "imgui_ui.hpp"

#include <glm/glm.hpp>
#include <imgui.h>
#include <memory>
#include <stdint.h>

void ImguiUI::RenderUI()
{
    if (ImGui::Begin("Status"))
    {
        ImGui::Text("FPS: %d", fps);
        ImGui::End();
    }
}

void ImguiUI::UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                         gecs::resource<Yutrel::Time> time)
{
    auto imgui_ui = std::reinterpret_pointer_cast<ImguiUI>(ui->ui);

    imgui_ui->fps = time->GetFPS();
}