#include "editor.h"

#include "Yutrel/function/global/global_context.h"
#include "editor/ui/editor_ui.h"

namespace Yutrel
{
    YutrelEditor::YutrelEditor(YutrelEngine *engine)
        : Application(engine)
    {
    }

    void YutrelEditor::initialize()
    {
        // imgui
        m_editor_ui                   = std::make_shared<EditorUI>();
        WindowUIInitInfo ui_init_info = {
            g_runtime_global_context.m_window_system,
            g_runtime_global_context.m_render_system};
        m_editor_ui->initialize(ui_init_info);
    }

    void YutrelEditor::tick()
    {
    }

    void YutrelEditor::clear()
    {
    }
} // namespace Yutrel