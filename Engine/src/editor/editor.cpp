#include "yutrel_pch.h"

#include "editor.h"

#include "editor/global/editor_global_context.h"
#include "editor/ui/editor_ui.h"
#include "runtime/function/global/global_context.h"

namespace Yutrel
{
    void YutrelEditor::initialize(YutrelEngine *engine)
    {
        assert(engine);

        m_engine_runtime = engine;

        // editor global context
        EditorGlobalContextInitInfo init_info = {
            g_runtime_global_context.m_window_system.get(),
            g_runtime_global_context.m_render_system.get(),
            engine};
        g_editor_global_context.initialize(init_info);

        // imgui
        m_editor_ui                   = std::make_shared<EditorUI>();
        WindowUIInitInfo ui_init_info = {
            g_runtime_global_context.m_window_system,
            g_runtime_global_context.m_render_system};
        m_editor_ui->initialize(ui_init_info);
    }

    /**
     * editor run的过程
     * 就是计算delta time并tick engine
     */
    void YutrelEditor::run()
    {
        assert(m_engine_runtime);
        assert(m_editor_ui);

        float delta_time;
        while (true)
        {
            delta_time = m_engine_runtime->calculateDeltaTime();
            // todo scene manager tick
            // todo input manager tick
            if (!m_engine_runtime->tickOneFrame(delta_time))
            {
                return;
            }
        }
    }

    void YutrelEditor::clear()
    {
        g_editor_global_context.clear();
    }
} // namespace Yutrel