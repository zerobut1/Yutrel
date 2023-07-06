#include <Yutrel/Yutrel.hpp>

#include "sandbox.hpp"

#include <iostream>

int main()
{
    auto app = Yutrel::CreateApplication();

    app->GetWorld()
        .AddStartupSystem(SpawnCamera)
        // .AddStartupSystem(SpawnFramebuffer)
        .AddStartupSystem(SpawnLight)
        .AddStartupSystem(SpawnScene)
        .AddStartupSystem(SpawnSkybox)
        .AddSystem(DrawScene);

    app->Init();

    app->Run();

    app->Shutdown();
}