#pragma once

#include "editor/ui/editor_ui.h"

#include <Yutrel/core/application/application.h>

namespace Yutrel
{
    class YutrelEditor : public Application
    {
    public:
        YutrelEditor(YutrelEngine *engine);
        virtual ~YutrelEditor() = default;

        virtual void initialize() override;
        virtual void tick() override;
        virtual void clear() override;

    private:
        std::shared_ptr<EditorUI> m_editor_ui;
    };
} // namespace Yutrel