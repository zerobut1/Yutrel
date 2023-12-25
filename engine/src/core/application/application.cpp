#include "yutrel_pch.hpp"

#include "application.hpp"

#include "core/path/path.hpp"
#include "function/input/input.hpp"
#include "function/render/renderer.hpp"
#include "function/window/window.hpp"
// #include "core/time/time.hpp"

namespace Yutrel
{
    // 单例类设计模式，创建/获取单例
    Application& Application::Create()
    {
        static Application instance;
        return instance;
    }

    // 默认的资源以及事件的创建
    // 返回World&以方便链式调用
    // World& Application::AddDefaultPlugin()
    // {
    // m_world
    //     .SetResource(ExitTrigger())
    //     .SetResource(Window::Create("Yutrel", 1920, 1080))
    //     .SetResource(Input::Create())
    //     .SetResource(Time::Create())
    //     .SetResource(Renderer::Create(*m_world.GetResource<Ref<Window>>()))
    //     .SetResource(Paths())
    //     .AddStartupSystem(Renderer::InitRenderData)
    //     .AddSystem(Window::Update)
    //     .AddSystem(Time::Update)
    //     .AddSystem(ExitTrigger::DetectShouldExit);

    // return m_world;
    // }

    Application& Application::Init(const std::string& title /* = "Yutrel App"*/, uint32_t width /* = 1920*/, uint32_t height /* = 1080*/)
    {
        LOG_INFO("Initlizing");

        auto& reg = m_world.regist_registry(title);
        m_world.start_with(title);

        // 默认资源
        auto cmds = reg.commands();
        cmds.emplace_resource<WindowResource>(title, width, height);
        cmds.emplace_resource<Input>();
        cmds.emplace_resource<RenderResource>();

        // 默认系统
        reg.regist_startup_system<Input::Init>()
            .regist_startup_system<RenderResource::Init>()
            .regist_update_system<WindowResource::Update>()
            .regist_update_system<RenderResource::Update>();

        return *this;
    }

    void Application::Run()
    {
        m_world.startup();

        LOG_INFO("Start Running");
        auto window = m_world.res<WindowResource>()->GetWindow();
        // 主循环
        while (!window->ShouldClose())
        {
            m_world.update();
        }

        LOG_INFO("Shutdown");
        m_world.shutdown();
    }

} // namespace Yutrel