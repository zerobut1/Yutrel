#include "yutrel_pch.hpp"

#include "application.hpp"

#include "core/time/time.hpp"
#include "function/input/input.hpp"
#include "function/render/renderer.hpp"
#include "function/window/window.hpp"
#include "resource/asset/asset.hpp"
#include "resource/component/component.hpp"
#include "resource/component/window_ui.hpp"

namespace Yutrel
{
    Application& Application::Create()
    {
        static Application instance;
        return instance;
    }

    Application& Application::Init(const std::string& title /* = "Yutrel App"*/, uint32_t width /* = 1920*/, uint32_t height /* = 1080*/)
    {
        LOG_INFO("Initlizing");

        auto& reg = m_world.regist_registry(title);
        m_world.start_with(title);

        // 默认资源
        auto cmds = reg.commands();
        cmds.emplace_resource<Time>();
        cmds.emplace_resource<WindowResource>(title, width, height);
        cmds.emplace_resource<Input>();
        cmds.emplace_resource<RendererResource>();
        cmds.emplace_resource<AssetManager>();

        // 默认系统
        reg.regist_startup_system<Input::Init>()
            .regist_startup_system<RendererResource::Init>()
            .regist_startup_system<UIResource::Init>()
            .regist_update_system<Time::Update>()
            .regist_update_system<WindowResource::Update>()
            .regist_update_system<AssetManager::LoadGLTFScenes>()
            .regist_update_system<AssetManager::LoadMeshes>()
            .regist_update_system<AssetManager::LoadMaterials>()
            .regist_update_system<AssetManager::LoadSkyboxes>()
            .regist_update_system<AssetManager::LoadWater>()
            .regist_update_system<RendererResource::Update>();

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