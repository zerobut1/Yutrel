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
        EditorUI();
        virtual ~EditorUI() = default;

        virtual void initialize(WindowUIInitInfo init_info) override final;
        virtual void preRender() override final;

        virtual void setEngineOutputTextureID(uint32_t id) override { texture_id = id; }

        virtual std::array<int, 2> getViewport() override { return {(int)m_viewport_size.x, (int)m_viewport_size.y}; }

    private:
        void showEditorUI();
        void showEditorMenu(bool *p_open);
        void showEditorGameWindow(bool *p_open);
        void showEditorDetailWindow(bool *p_open);

    private:
        // 顶部菜单栏
        bool m_editor_menu_window_open = true;
        // 渲染画面显示窗口
        bool m_game_engine_window_open = true;
        // 设置信息窗口
        bool m_detail_window_open = true;

        ImVec2 m_viewport_size{1920.0f, 1080.0f};
        uint64_t texture_id;
    };
} // namespace Yutrel