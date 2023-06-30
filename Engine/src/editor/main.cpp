#include <Yutrel/Yutrel.hpp>

#include "sandbox.hpp"

#include <iostream>

int main()
{
    auto app = Yutrel::CreateApplication();

    app->GetWorld()
        .AddStartupSystem(SpawnCamera)
        // .AddStartupSystem(SpawnFramebuffer)
        .AddStartupSystem(SpawnTextureShader)
        .AddStartupSystem(SpawnBackpack)
        .AddStartupSystem(SpawnSkybox)
        .AddSystem(DrawScene);

    app->Init();

    app->Run();

    app->Shutdown();
}