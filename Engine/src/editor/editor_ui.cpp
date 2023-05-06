#include "editor_ui.h"

#include "editor_global_context.h"
#include "runtime/function/global/global_context.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>

#include <array>
#include <utility>

namespace Yutrel
{
    std::vector<std::pair<std::string, bool>> g_editor_node_state_array;
    int g_node_depth = -1;

    EditorUI::EditorUI()
    {
    }

    void EditorUI::initialize(WindowUIInitInfo init_info)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        float x_scale, y_scale;
        glfwGetWindowContentScale(init_info.window_system->getglfwWindow(), &x_scale, &y_scale);
        float content_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingAlwaysTabBar         = true;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.Fonts->AddFontFromFileTTF(
            "C:\\Windows\\Fonts\\msyh.ttc",
            32.0f,
            nullptr,
            nullptr);
        io.Fonts->Build();

        
        ImGuiStyle &style     = ImGui::GetStyle();
        style.WindowPadding   = ImVec2(1.0, 0);
        style.FramePadding    = ImVec2(14.0, 2.0f);
        style.ChildBorderSize = 0.0f;
        style.FrameRounding   = 5.0f;
        style.FrameBorderSize = 1.5f;

        init_info.render_system->initializeUIRenderBackend(this);
    }

    void EditorUI::preRender()
    {
        showEditorUI();
    }

    void EditorUI::showEditorUI()
    {
        showEditorMenu(&m_editor_menu_window_open);
        showEditorGameWindow(&m_game_engine_window_open);
        showEditorDetailWindow(&m_detail_window_open);
    }

    void EditorUI::showEditorMenu(bool *p_open)
    {
        ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_DockSpace;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                        ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

        const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
        std::array<int, 2> window_size = g_editor_global_context.m_window_system->getWindowSize();
        ImGui::SetNextWindowSize(ImVec2((float)window_size[0], (float)window_size[1]), ImGuiCond_Always);

        ImGui::SetNextWindowViewport(main_viewport->ID);

        ImGui::Begin("Editor menu", p_open, window_flags);

        ImGuiID main_docking_id = ImGui::GetID("Main Docking");
        if (ImGui::DockBuilderGetNode(main_docking_id) == nullptr)
        {
            ImGui::DockBuilderAddNode(main_docking_id, dock_flags);
            ImGui::DockBuilderSetNodePos(main_docking_id,
                                         ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 18.0f));
            ImGui::DockBuilderSetNodeSize(main_docking_id,
                                          ImVec2((float)window_size[0], (float)window_size[1] - 18.0f));

            ImGuiID center = main_docking_id;
            ImGuiID left;
            ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.3f, nullptr, &left);

            ImGuiID left_other;
            ImGuiID left_file_content = ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.30f, nullptr, &left_other);

            ImGuiID left_game_engine;
            ImGuiID left_asset =
                ImGui::DockBuilderSplitNode(left_other, ImGuiDir_Left, 0.30f, nullptr, &left_game_engine);

            // ImGui::DockBuilderDockWindow("World Objects", left_asset);
            ImGui::DockBuilderDockWindow("Inspector", right);
            // ImGui::DockBuilderDockWindow("File Content", left_file_content);
            ImGui::DockBuilderDockWindow("Scene View", left_game_engine);

            ImGui::DockBuilderFinish(main_docking_id);
        }

        ImGui::DockSpace(main_docking_id);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Window"))
            {
                // ImGui::MenuItem("World Objects", nullptr, &m_asset_window_open);
                ImGui::MenuItem("Scene View", nullptr, &m_game_engine_window_open);
                // ImGui::MenuItem("File Content", nullptr, &m_file_content_window_open);
                ImGui::MenuItem("Inspector", nullptr, &m_detail_window_open);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorUI::showEditorGameWindow(bool *p_open)
    {
        ImGuiIO &io                   = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize;

        const ImGuiViewport *main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("Scene View", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        glm::vec2 render_target_window_pos  = {0.0f, 0.0f};
        glm::vec2 render_target_window_size = {0.0f, 0.0f};

        render_target_window_pos.x  = ImGui::GetWindowPos().x;
        render_target_window_pos.y  = ImGui::GetWindowPos().y;
        render_target_window_size.x = ImGui::GetWindowSize().x;
        render_target_window_size.y = ImGui::GetWindowSize().y;

        g_runtime_global_context.m_render_system->updateEngineContentViewport(
            render_target_window_pos.x,
            render_target_window_pos.y,
            render_target_window_size.x,
            render_target_window_size.y);

        EngineContentViewport render_target_viewport;
        render_target_viewport = g_runtime_global_context.m_render_system->getEngineContentViewport();

        // g_editor_global_context.m_input_manager->setEngineWindowPos(render_target_window_pos);
        // g_editor_global_context.m_input_manager->setEngineWindowSize(render_target_window_size);

        uint64_t texture_id = g_runtime_global_context.m_render_system->getTexColorBuffer();
        ImGui::GetWindowDrawList()->AddImage((void *)texture_id,
                                             ImVec2(render_target_window_pos.x, render_target_window_pos.y),
                                             ImVec2(render_target_window_size.x + render_target_window_pos.x,
                                                    render_target_window_size.y + render_target_window_pos.y),
                                             ImVec2(0, 1),
                                             ImVec2(1, 0));

        ImGui::End();
    }

    void EditorUI::showEditorDetailWindow(bool *p_open)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const ImGuiViewport *main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("Inspector", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        // Inspector
        ImGuiIO &io = ImGui::GetIO();

        /*
         * 会填充设置的，以后吧
         */

        ImGui::End();
    }

    std::string EditorUI::getLeafUINodeParentLabel()
    {
        std::string parent_label;
        int array_size = g_editor_node_state_array.size();
        for (int index = 0; index < array_size; index++)
        {
            parent_label += g_editor_node_state_array[index].first + "::";
        }
        return parent_label;
    }

} // namespace Yutrel