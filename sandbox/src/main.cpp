// #include "scene_pbr.hpp"

#include <Yutrel.hpp>

void test1()
{
    LOG_WARN("测试沙盒");
}

void test2(gecs::resource<Yutrel::Input> input)
{
    if (input->IsKeyPressed(Yutrel::Key::A))
    {
        LOG_WARN("A is Pressed");
    }

    if (input->IsMousePressed(Yutrel::Mouse::ButtonLeft))
    {
        LOG_WARN("按下左键");
    }

    // LOG_WARN("{0},{1}", input->GetCursorPos().x, input->GetCursorPos().y);
}

int main()
{
    // app.AddDefaultPlugin()
    //     .SetResource(Yutrel::CreateRef<EditorUI>(1920, 1080))
    //     .SetResource(Framebuffers(1902, 1080))
    //     .SetResource(Shaders())
    //     .SetResource(UniformBuffers())
    //     .AddStartupSystem(SpawnCamera)
    //     .AddStartupSystem(SpawnLight)
    //     .AddStartupSystem(SpawnSkybox)
    //     .AddStartupSystem(SpawnPBRScene)
    //     .AddSystem(UpdateScene)
    //     .AddSystem(DrawPBRScene)
    //     .AddSystem(DrawUI);

    Yutrel::Application::Create()
        .Init("Sandbox", 1920, 1080)
        .AddStartupSystem<test1>()
        .AddSystem<test2>()
        .Run();

    return 0;
}