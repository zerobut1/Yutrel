#pragma once

#include "Yutrel/core/engine.h"

namespace Yutrel
{
    class Application
    {
    public:
        Application(YutrelEngine *engine);
        virtual ~Application() = default;

        virtual void initialize() = 0;
        virtual void clear()      = 0;

        void run();

    protected:
        virtual void tick() = 0;

    protected:
        YutrelEngine *m_engine_runtime;
    };

    Application *CreateApplication(YutrelEngine *engine);
} // namespace Yutrel