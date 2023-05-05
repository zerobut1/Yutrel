#pragma once

#include "runtime/engine.h"
#include "editor_ui.h"

#include <memory>

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
        std::shared_ptr<EditorUI> m_editor_ui;
        YutrelEngine *m_engine_runtime;
    };
} // namespace Yutrel