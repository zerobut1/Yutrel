#pragma once

#include "editor/editor.h"

#include <Yutrel/Yutrel.h>

#include <glm/fwd.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

namespace Yutrel
{
    class EditorUI : public WindowUI
    {
    public:
        virtual void initialize() override final;
        virtual void preRender() override final;
        virtual void clear() override final;

    private:
        void showEditorUI();
        void showMenuBar(bool *p_open);
        void showGameViewport(bool *p_open);
        void showSettingsWindow(bool *p_open);

    private:
        // 顶部菜单栏
        bool m_menu_bar_open = true;
        // 渲染画面显示窗口
        bool m_game_engine_viewport_open = true;
        // 设置信息窗口
        bool m_game_settings_open = true;
    };
} // namespace Yutrel