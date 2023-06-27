#pragma once

#include "Yutrel/core/ecs/ecs.hpp"

namespace Yutrel
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        static inline Application& Get() { return *s_instance; }

        World& GetWorld() { return m_world; }

        void Init();
        void Run();
        void Shutdown();

    private:
        World m_world;

        bool m_running = true;

        static Application* s_instance;
    };

    class ExitTrigger final
    {
    public:
        bool ShouldExit() const { return m_should_exit; }

        void Exit() { m_should_exit = true; }

        static void DetectShouldExit(Commands& cmd, Querier querier,
                                     Resources resources, Events& events);

    private:
        bool m_should_exit = false;
    };

} // namespace Yutrel

// #pragma once

// #include "Yutrel/core/engine.h"

// namespace Yutrel
// {
//     class Application
//     {
//     public:
//         Application(YutrelEngine *engine);
//         virtual ~Application() = default;

//         virtual void initialize() = 0;
//         virtual void clear()      = 0;

//         void run();

//     protected:
//         virtual void tick(float delta_time) = 0;

//     protected:
//         YutrelEngine *m_engine;

//         std::shared_ptr<WindowUI> m_ui;

//     private:
//         bool m_running = true;
//     };

//     Application *CreateApplication(YutrelEngine *engine);
// } // namespace Yutrel