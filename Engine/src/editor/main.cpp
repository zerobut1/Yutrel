#include <Yutrel/Yutrel.hpp>

#include "sandbox.hpp"

#include <iostream>

int main()
{
    auto app = Yutrel::CreateApplication();

    app->GetWorld()
        .AddStartupSystem(SpawnViewport)
        .AddStartupSystem(SpawnCamera)
        .AddStartupSystem(SpawnLight)
        .AddStartupSystem(SpawnScene)
        .AddStartupSystem(SpawnSkybox)
        .AddStartupSystem(SpawnShadowmap)
        .AddStartupSystem(SpawnUI)
        .AddSystem(UpdateScene)
        .AddSystem(DrawScene)
        .AddSystem(DrawUI);

    app->Init();

    app->Run();

    app->Shutdown();
}