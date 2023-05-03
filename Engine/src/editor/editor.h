#pragma once

#include "runtime/engine.h"

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

    private:
        YutrelEngine *m_engine_runtime;
    };
} // namespace Yutrel