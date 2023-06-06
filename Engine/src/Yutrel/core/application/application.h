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
        virtual void tick(float delta_time) = 0;

    protected:
        YutrelEngine *m_engine;

        std::shared_ptr<WindowUI> m_ui;

    private:
        bool m_running = true;
    };

    Application *CreateApplication(YutrelEngine *engine);
} // namespace Yutrel