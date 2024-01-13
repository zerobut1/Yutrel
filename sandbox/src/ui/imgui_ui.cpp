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

    if (ImGui::Begin("background"))
    {
        ImGui::InputFloat4("data1", (float*)&background.data1);
        ImGui::InputFloat4("data2", (float*)&background.data2);
        ImGui::InputFloat4("data3", (float*)&background.data3);
        ImGui::InputFloat4("data4", (float*)&background.data4);

        ImGui::End();
    }
}

void ImguiUI::UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                         gecs::resource<Yutrel::EngineStatus> status,
                         gecs::resource<gecs::mut<Yutrel::BackGroundColor>> background_color)
{
    auto imgui_ui = std::reinterpret_pointer_cast<ImguiUI>(ui->ui);

    // 每0.5秒更新一次
    static uint32_t frame_count = 0;
    static float duration       = 0.0f;

    frame_count++;
    duration += status->frametime;
    if (duration >= 500.0f)
    {
        imgui_ui->fps    = (frame_count / duration) * 1000;
        imgui_ui->status = status.get();
        frame_count      = 0;
        duration         = 0.0f;
    }

    background_color.get() = imgui_ui->background;
}