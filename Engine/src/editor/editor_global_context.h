#pragma once

#include "editor_input_manager.h"

namespace Yutrel
{
    struct EditorGlobalContextInitInfo
    {
        class WindowSystem *window_system;
        class RenderSystem *render_system;
        class YutrelEngine *engine_runtime;
    };

    class EditorGlobalContext
    {
    public:
        // EditorSceneManager *m_scene_manager{nullptr};
        // EditorInputManager *m_input_manager{nullptr};
        RenderSystem *m_render_system{nullptr};
        WindowSystem *m_window_system{nullptr};
        YutrelEngine *m_engine_runtime{nullptr};

    public:
        void initialize(const EditorGlobalContextInitInfo &init_info);
        void clear();
    };

    extern EditorGlobalContext g_editor_global_context;
} // namespace Yutrel