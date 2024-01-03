#include <Yutrel.hpp>

#include "ui/imgui_ui.hpp"

void StartWorld(gecs::commands cmds,
                gecs::resource<gecs::mut<Yutrel::AssetManager>> asset_manager)
{
    auto entity = cmds.create();
    cmds.emplace<Yutrel::PbrBundle>(
        entity,
        Yutrel::PbrBundle{
            // asset_manager->LoadMesh("resource/lost_empire/lost_empire.obj"),
            asset_manager->LoadMesh("resource/viking_room/viking_room.obj"),
        });
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
        .AddResource<Yutrel::UIResource>(Yutrel::CreateRef<ImguiUI>())
        .AddResource<Yutrel::BackGroundColor>()
        .AddStartupSystem<StartWorld>()
        .AddSystem<ImguiUI::UpdateData>()
        .Run();

    return 0;
}