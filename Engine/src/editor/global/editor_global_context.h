#pragma once

// #include "editor_input_manager.h"

namespace Yutrel
{
    struct EditorGlobalContextInitInfo
    {
        class WindowSystem *window_system;
        class RenderSystem *render_system;
        class YutrelEngine *engine_runtime;
    };

    // 编辑器的context
    // 目前作用貌似不大
    class EditorGlobalContext
    {
    public:
        void initialize(const EditorGlobalContextInitInfo &init_info);
        void clear();

    public:
        // EditorSceneManager *m_scene_manager{nullptr};
        // EditorInputManager *m_input_manager{nullptr};
        RenderSystem *m_render_system;
        WindowSystem *m_window_system;
        YutrelEngine *m_engine_runtime;
    };

    extern EditorGlobalContext g_editor_global_context;
} // namespace Yutrel