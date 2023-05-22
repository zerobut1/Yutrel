#pragma once

#include "runtime/engine.h"
#include "editor/ui/editor_ui.h"

namespace Yutrel
{
    class YutrelEditor
    {
    public:
        YutrelEditor()          = default;
        virtual ~YutrelEditor() = default;

        void initialize(YutrelEngine *engine);
        void run();
        void clear();

    protected:
        //这里为什么是protected暂时没想明白
        std::shared_ptr<EditorUI> m_editor_ui;
        YutrelEngine *m_engine_runtime;
    };
} // namespace Yutrel