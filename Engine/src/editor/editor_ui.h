#pragma once

#include "runtime/function/render/render_system.h"
#include "runtime/function/ui/window_ui.h"

#include <functional>
#include <map>
#include <string>
#include <unordered_map>

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
        bool m_editor_menu_window_open = true;
        bool m_game_engine_window_open = true;
        bool m_detail_window_open      = true;
    };
} // namespace Yutrel