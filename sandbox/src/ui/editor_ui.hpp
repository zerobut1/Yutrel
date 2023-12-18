#pragma once
#include <Yutrel.hpp>

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <stdint.h>

struct Viewport
{
    int width;
    int height;
};

class EditorUI : public Yutrel::WindowUI
{
public:
    EditorUI(uint32_t width, uint32_t height);
    virtual ~EditorUI();

    virtual void RenderUI() override final;

    Viewport GetViewport() const
    {
        return m_viewport;
    }

private:
    void ShowEditorUI();
    void ShowMenuBar(bool* p_open);
    void ShowGameViewport(bool* p_open);
    void ShowSettingsWindow(bool* p_open);

private:
    Viewport m_viewport;

    // 顶部菜单栏
    bool m_menu_bar_open = true;
    // 渲染画面显示窗口
    bool m_game_engine_viewport_open = true;
    // 设置信息窗口
    bool m_game_settings_open = true;
};