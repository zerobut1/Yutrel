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
        ImGui::Text("frametime %f ms", status.frametime);
        ImGui::Text("draw time %f ms", status.mesh_draw_time);
        ImGui::Text("update time %f ms", status.renderer_tick_time);
        ImGui::Text("triangles %i", status.triangle_count);
        ImGui::Text("draws %i", status.drawcall_count);
        ImGui::End();
    }
}

void ImguiUI::UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                         gecs::resource<Yutrel::EngineStatus> status,
                         gecs::resource<Yutrel::Time> time)
{
    auto imgui_ui = std::reinterpret_pointer_cast<ImguiUI>(ui->ui);

    // 每0.5秒更新一次
    static float duration = 0.0f;

    duration += status->frametime;
    if (duration >= 500.0f)
    {
        imgui_ui->status = status.get();
        duration         = 0.0f;
    }

    imgui_ui->fps = time->GetFPS();
}