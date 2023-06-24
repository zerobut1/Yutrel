#include "editor_ui.h"

#include <Yutrel./Yutrel.h>

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <imgui.h>

namespace Yutrel
{
    void EditorUI::initialize()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigDockingAlwaysTabBar         = true;
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        // 暂时用比较暴力的方法设置字体
        io.Fonts->AddFontFromFileTTF(
            "C:\\Windows\\Fonts\\msyh.ttc",
            32.0f);

        // todo :搞一个好看的UI
        ImGui::StyleColorsDark();

        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        YutrelEditor::get().m_engine->getRenderSystem()->initializeUIRender(this);
    }

    void EditorUI::preRender()
    {
        showEditorUI();
    }

    void EditorUI::clear()
    {
        YutrelEditor::get().m_engine->getRenderSystem()->clearUIRender(this);
    }

    void EditorUI::showEditorUI()
    {
        // 暂时只显示这三部分
        showMenuBar(&m_menu_bar_open);
        showGameViewport(&m_game_engine_viewport_open);
        showSettingsWindow(&m_game_settings_open);
    }

    void EditorUI::showMenuBar(bool *p_open)
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", p_open, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        else
        {
            LOG_ERROR("Dockspace Disabled!");
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                ImGui::MenuItem("Viewport", NULL, &m_game_engine_viewport_open);
                ImGui::MenuItem("Settings", NULL, &m_game_settings_open);
                // ImGui::Separator();
                // todo : close
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorUI::showGameViewport(bool *p_open)
    {
        if (!*p_open)
            return;

        ImGui::Begin("Viewport");

        YutrelEditor::get().m_viewport_focused = ImGui::IsWindowFocused();

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        YutrelEditor::get().m_viewport_size.x = viewportPanelSize.x;
        YutrelEditor::get().m_viewport_size.y = viewportPanelSize.y;

        uint64_t texture_ID = YutrelEditor::get().m_viewport_framebuffer->getColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void *>(texture_ID), ImVec2{viewportPanelSize.x, viewportPanelSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

        ImGui::End();
    }

    void EditorUI::showSettingsWindow(bool *p_open)
    {
        if (!*p_open)
            return;

        ImGui::Begin("Settings");

        ImGui::SliderFloat3("light_pos", (float *)&YutrelEditor::get().lightPos, -5.0f, 5.0f);

        ImGui::End();
    }

} // namespace Yutrel