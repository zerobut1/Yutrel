#pragma once
#include <Yutrel/Yutrel.hpp>

#include <glm/fwd.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

struct Viewport
{
    int width;
    int height;
};

class EditorUI : public Yutrel::WindowUI
{
public:
    EditorUI();
    virtual ~EditorUI();

    virtual void RenderUI() override final;

private:
    void ShowEditorUI();
    void ShowMenuBar(bool* p_open);
    void ShowGameViewport(bool* p_open);
    void ShowSettingsWindow(bool* p_open);

private:
    // 顶部菜单栏
    bool m_menu_bar_open = true;
    // 渲染画面显示窗口
    bool m_game_engine_viewport_open = true;
    // 设置信息窗口
    bool m_game_settings_open = true;
};