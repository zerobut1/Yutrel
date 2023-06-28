#pragma once

#include "Yutrel/core/ecs/ecs.hpp"

namespace Yutrel
{
    /*
     * 引擎的入口
     */
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

        static Application* s_instance;
    };

    // 应当用此函数创建app
    Application* CreateApplication();

    /*
     * 退出检测
     */
    class ExitTrigger final
    {
    public:
        void Exit() { m_should_exit = true; }
        bool ShouldExit() const { return m_should_exit; }

        static void DetectShouldExit(Commands& cmd, Querier querier, Resources resources, Events& events);

    private:
        bool m_should_exit = false;
    };

} // namespace Yutrel