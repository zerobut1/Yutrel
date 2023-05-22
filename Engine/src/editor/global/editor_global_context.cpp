#include "yutrel_pch.h"

#include "editor_global_context.h"

// #include "editor_input_manager.h"

namespace Yutrel
{
    EditorGlobalContext g_editor_global_context;

    //感觉这个东西初始化得很奇怪，以后想办法改掉
    void EditorGlobalContext::initialize(const EditorGlobalContextInitInfo &init_info)
    {
        g_editor_global_context.m_engine_runtime = init_info.engine_runtime;
        g_editor_global_context.m_window_system  = init_info.window_system;
        g_editor_global_context.m_render_system  = init_info.render_system;

        // m_scene_manager = new EditorSceneManager();
        // m_input_manager = new EditorInputManager();
        // m_scene_manager->initialize();
        // m_input_manager->initialize();
    }

    void EditorGlobalContext::clear()
    {
        // delete (m_scene_manager);
        // delete (m_input_manager);
    }
} // namespace Yutrel