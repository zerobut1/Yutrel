// #include "editor_ui.hpp"

// #include "glm/geometric.hpp"
// #include "imgui.h"
// #include "scene_pbr.hpp"
// #include <memory>

// EditorUI::EditorUI(uint32_t width, uint32_t height)
// {
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();

//     ImGuiIO& io = ImGui::GetIO();
//     io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
//     // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
//     io.ConfigDockingAlwaysTabBar         = true;
//     io.ConfigWindowsMoveFromTitleBarOnly = true;

//     // 暂时用比较暴力的方法设置字体
//     io.Fonts->AddFontFromFileTTF(
//         "C:\\Windows\\Fonts\\msyh.ttc",
//         32.0f);

//     // todo :搞一个好看的UI
//     ImGui::StyleColorsDark();

//     ImGuiStyle& style = ImGui::GetStyle();
//     if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//     {
//         style.WindowRounding              = 0.0f;
//         style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//     }

//     // Yutrel::Resources resources = Yutrel::Resources(Yutrel::Application::GetInstance().GetWorld());
//     // auto renderer               = resources.Get<Yutrel::Ref<Yutrel::Renderer>>();
//     // renderer->InitializeUIRender(this);

//     // m_viewport.width  = width;
//     // m_viewport.height = height;
// }

// EditorUI::~EditorUI()
// {
//     // Yutrel::Resources resources = Yutrel::Resources(Yutrel::Application::GetInstance().GetWorld());
//     // auto renderer               = resources.Get<Yutrel::Ref<Yutrel::Renderer>>();
//     // renderer->ClearUIRender(this);
// }

// void EditorUI::RenderUI()
// {
//     ShowEditorUI();
// }

// void EditorUI::ShowEditorUI()
// {
//     // 暂时只显示这三部分
//     ShowMenuBar(&m_menu_bar_open);
//     ShowGameViewport(&m_game_engine_viewport_open);
//     ShowSettingsWindow(&m_game_settings_open);
// }

// void EditorUI::ShowMenuBar(bool* p_open)
// {
//     static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

//     // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
//     // because it would be confusing to have two docking targets within each others.
//     ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

//     const ImGuiViewport* viewport = ImGui::GetMainViewport();
//     ImGui::SetNextWindowPos(viewport->WorkPos);
//     ImGui::SetNextWindowSize(viewport->WorkSize);
//     ImGui::SetNextWindowViewport(viewport->ID);
//     ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//     ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//     window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
//     window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

//     // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
//     // and handle the pass-thru hole, so we ask Begin() to not render a background.
//     if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
//         window_flags |= ImGuiWindowFlags_NoBackground;

//     // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
//     // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
//     // all active windows docked into it will lose their parent and become undocked.
//     // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
//     // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
//     ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//     ImGui::Begin("DockSpace Demo", p_open, window_flags);
//     ImGui::PopStyleVar();
//     ImGui::PopStyleVar(2);

//     // Submit the DockSpace
//     ImGuiIO& io = ImGui::GetIO();
//     if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
//     {
//         ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//         ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
//     }
//     else
//     {
//         LOG_ERROR("Dockspace Disabled!");
//     }

//     if (ImGui::BeginMenuBar())
//     {
//         if (ImGui::BeginMenu("Options"))
//         {
//             ImGui::MenuItem("Viewport", NULL, &m_game_engine_viewport_open);
//             ImGui::MenuItem("Settings", NULL, &m_game_settings_open);
//             // ImGui::Separator();
//             // todo : close
//             ImGui::EndMenu();
//         }
//         ImGui::EndMenuBar();
//     }

//     ImGui::End();
// }

// void EditorUI::ShowGameViewport(bool* p_open)
// {
//     if (!*p_open)
//         return;

//     ImGui::Begin("Viewport");

//     // 更新viewport
//     ImVec2 viewportPanelSize  = ImGui::GetContentRegionAvail();
//     m_viewport.width          = viewportPanelSize.x;
//     m_viewport.height         = viewportPanelSize.y;
//     // auto resources            = Yutrel::Resources(Yutrel::Application::GetInstance().GetWorld());
//     // auto viewport_framebuffer = resources.Get<Framebuffers>().viewport;

//     // uint64_t texture_ID = viewport_framebuffer->GetColorTexture(0)->GetRendererID();
//     // ImGui::Image(reinterpret_cast<void*>(texture_ID), ImVec2{viewportPanelSize.x, viewportPanelSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

//     ImGui::End();
// }

// void EditorUI::ShowSettingsWindow(bool* p_open)
// {
//     if (!*p_open)
//         return;

//     ImGui::Begin("Settings");

//     // auto entities       = querier.Query<Yutrel::PointLight>();
//     // auto& light         = querier.Get<Yutrel::PointLight>(entities[0]);
//     // entities            = querier.Query<Yutrel::SkyBox>();
//     // auto SkyBox         = querier.Get<Yutrel::SkyBox>(entities[0]);
//     // uint64_t texture_ID = SkyBox.environment_map->GetRendererID();

//     //  ImGui::Image(reinterpret_cast<void*>(texture_ID), {500, 500});

//     // auto querier   = Yutrel::Querier(Yutrel::Application::GetInstance().GetWorld());
//     // auto resources = Yutrel::Resources(Yutrel::Application::GetInstance().GetWorld());

//     // 显示fps
//     // auto time = resources.Get<Yutrel::Ref<Yutrel::Time>>();
//     ImGui::Text("FPS: ");
//     ImGui::SameLine(0.0f, 0.0f);
//     // ImGui::Text("%d", (int)time->GetFPS());
//     ImGui::SameLine(0.0f, 0.0f);
//     // ImGui::Text("(%.2f ms)", time->GetMS());

//     // 重新编译shader
//     // auto& shaders = resources.Get<Shaders>();
//     if (ImGui::Button("Compile Shader"))
//     {
//         // shaders = Shaders();
//     }

//     // 调整ibl
//     // auto skybox_entity = querier.Query<Yutrel::SkyBox>()[0];
//     // auto& skybox       = querier.Get<Yutrel::SkyBox>(skybox_entity);
//     // ImGui::SliderFloat("Skybox Exposure", &skybox.exposure, 0.0f, 2.0f);

//     // // 调整太阳
//     // auto& sun = querier.Get<Yutrel::DirectionLight>(querier.Query<Yutrel::DirectionLight>()[0]);

//     // ImGui::SliderFloat3("Sun Direction", (float*)&sun.direction, -1.0f, 1.0f);
//     // sun.direction = glm::normalize(sun.direction);
//     // ImGui::SliderFloat("Sun Intensity", &sun.intensity, 0.0f, 10.0f);

//     // // 调整点光源
//     // auto& point_light = querier.Get<Yutrel::PointLight>(querier.Query<Yutrel::PointLight>()[0]);
//     // ImGui::SliderFloat3("PointLight Color", (float*)&point_light.color, 0.0f, 1.0f);
//     // // sun.direction = glm::normalize(sun.direction);
//     // ImGui::SliderFloat("PointLight Intensity", &point_light.intensity, 0.0f, 100.0f);

//     ImGui::End();
// }