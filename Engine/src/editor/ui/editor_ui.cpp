#include "editor_ui.hpp"

// #include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <imgui.h>

<<<<<<< HEAD
namespace Yutrel
{
    void EditorUI::initialize()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
=======
EditorUI::EditorUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
>>>>>>> 787a764 (1.添加UI)

    ImGuiIO& io = ImGui::GetIO();
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

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Yutrel::Resources resources = Yutrel::Resources(Yutrel::Application::Get().GetWorld());
    auto renderer               = resources.Get<Yutrel::Renderer*>();
    renderer->InitializeUIRender(this);
}

EditorUI::~EditorUI()
{
    Yutrel::Resources resources = Yutrel::Resources(Yutrel::Application::Get().GetWorld());
    auto renderer               = resources.Get<Yutrel::Renderer*>();
    renderer->ClearUIRender(this);
}

void EditorUI::RenderUI()
{
    ShowEditorUI();
}

void EditorUI::ShowEditorUI()
{
    // 暂时只显示这三部分
    ShowMenuBar(&m_menu_bar_open);
    ShowGameViewport(&m_game_engine_viewport_open);
    ShowSettingsWindow(&m_game_settings_open);
}

void EditorUI::ShowMenuBar(bool* p_open)
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
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
    ImGuiIO& io = ImGui::GetIO();
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

void EditorUI::ShowGameViewport(bool* p_open)
{
    if (!*p_open)
        return;

    ImGui::Begin("Viewport");

<<<<<<< HEAD
        YutrelEditor::get().m_viewport_focused = ImGui::IsWindowFocused();

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
=======
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    Yutrel::Querier querier  = Yutrel::Querier(Yutrel::Application::Get().GetWorld());
    auto entities            = querier.Query<Viewport>();
    auto& viewport           = querier.Get<Viewport>(entities[0]);
    auto framebuffer         = querier.Get<Yutrel::Framebuffer*>(entities[0]);
>>>>>>> 787a764 (1.添加UI)

    viewport.width  = viewportPanelSize.x;
    viewport.height = viewportPanelSize.y;

    uint64_t texture_ID = framebuffer->getColorAttachmentRendererID();
    ImGui::Image(reinterpret_cast<void*>(texture_ID), ImVec2{viewportPanelSize.x, viewportPanelSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

    ImGui::End();
}

void EditorUI::ShowSettingsWindow(bool* p_open)
{
    if (!*p_open)
        return;

    ImGui::Begin("Settings");

    auto querier  = Yutrel::Querier(Yutrel::Application::Get().GetWorld());
    auto entities = querier.Query<Yutrel::Light>();
    auto& light   = querier.Get<Yutrel::Light>(entities[0]);

    ImGui::SliderFloat3("light_pos", (float*)&light.light_pos, -5.0f, 5.0f);

    ImGui::End();
}