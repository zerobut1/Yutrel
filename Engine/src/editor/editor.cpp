#include "editor.h"

#include "editor_global_context.h"
#include "editor_ui.h"
#include "runtime/function/global/global_context.h"


#include <memory>

namespace Yutrel
{
    void YutrelEditor::initialize(YutrelEngine *engine)
    {
        m_engine_runtime = engine;

        EditorGlobalContextInitInfo init_info = {g_runtime_global_context.m_window_system.get(),
                                                 g_runtime_global_context.m_render_system.get(),
                                                 engine};
        g_editor_global_context.initialize(init_info);

        m_editor_ui                   = std::make_shared<EditorUI>();
        WindowUIInitInfo ui_init_info = {
            g_runtime_global_context.m_window_system,
            g_runtime_global_context.m_render_system};
        m_editor_ui->initialize(ui_init_info);
    }

    void YutrelEditor::run()
    {
        while (true)
        {
            // todo 计算delta_time
            // todo editor_context 目前没有必要

            if (!m_engine_runtime->tickOneFrame())
            {
                return;
            }
        }
    }

    void YutrelEditor::clear()
    {
    }
} // namespace Yutrel