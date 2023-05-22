#pragma once

#include "runtime/function/ui/window_ui.h"

namespace Yutrel
{
    class EditorUI : public WindowUI
    {
    public:
        EditorUI();
        virtual ~EditorUI() = default;

        virtual void initialize(WindowUIInitInfo init_info) override final;
        virtual void preRender() override final;

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
    };
} // namespace Yutrel