#include "editor.h"

namespace Yutrel
{
    void YutrelEditor::initialize(YutrelEngine *engine)
    {
        m_engine_runtime = engine;

        // todo editor_context 目前没有必要
        // todo editor_ui
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