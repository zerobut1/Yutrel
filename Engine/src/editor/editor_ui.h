#pragma once

#include "runtime/function/render/render_system.h"
#include "runtime/function/ui/window_ui.h"

#include <map>
#include <string>
#include <unordered_map>
#include <functional>


namespace Yutrel
{
    class EditorUI : public WindowUI
    {
    public:
        EditorUI();

        virtual void initialize(WindowUIInitInfo init_info) override final;
        virtual void preRender() override final;

    private:
        // void onFileContentItemClicked(EditorFileNode *node);
        // void buildEditorFileAssetsUITree(EditorFileNode *node);
        void drawAxisToggleButton(const char *string_id, bool check_state, int axis_mode);
        // void createClassUI(Reflection::ReflectionInstance &instance);
        // void createLeafNodeUI(Reflection::ReflectionInstance &instance);
        std::string getLeafUINodeParentLabel();

        void showEditorUI();
        void showEditorMenu(bool *p_open);
        void showEditorWorldObjectsWindow(bool *p_open);
        void showEditorFileContentWindow(bool *p_open);
        void showEditorGameWindow(bool *p_open);
        void showEditorDetailWindow(bool *p_open);

        // void setUIColorStyle();

    private:
        std::unordered_map<std::string, std::function<void(std::string, void *)>> m_editor_ui_creator;

        bool m_editor_menu_window_open       = true;
        bool m_asset_window_open             = true;
        bool m_game_engine_window_open       = true;
        bool m_file_content_window_open      = true;
        bool m_detail_window_open            = true;
        bool m_scene_lights_window_open      = true;
        bool m_scene_lights_data_window_open = true;
    };
} // namespace Yutrel