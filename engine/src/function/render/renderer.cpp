#include "yutrel_pch.hpp"

#include "renderer.hpp"

#include "core/application/application.hpp"
#include "core/path/path.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include "resource/asset/asset.hpp"
#include "resource/asset/gltf_scene.hpp"
#include "resource/asset/material.hpp"
#include "resource/asset/mesh.hpp"
#include "resource/asset/texture.hpp"
#include "resource/component/camera/camera.hpp"
#include "resource/component/component.hpp"
#include "resource/component/window_ui.hpp"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    RendererResource::RendererResource()
    {
        renderer = CreateRef<VulkanRenderer>();
    }

    RendererResource::~RendererResource()
    {
        renderer->Clear();
        renderer.reset();
        LOG_INFO("Renderer Destroyed");
    }

    void RendererResource::Init(gecs::resource<RendererResource> render,
                                gecs::resource<WindowResource> window)
    {
        LOG_INFO("Initialize Render Resource");

        // 初始化renderer
        RendererInitInfo info{};
        info.raw_window = window->GetWindow()->GetRaw();
        info.width      = window->GetWidth();
        info.height     = window->GetHeight();

        render->renderer->Init(info);
    }

    void RendererResource::Update(gecs::querier<Ref<Mesh>, Ref<Material>, Transform> objects,
                                  gecs::querier<DirectionalLight> directional_lights,
                                  gecs::resource<RendererResource> render,
                                  gecs::resource<UIResource> ui,
                                  gecs::resource<Camera> camera,
                                  gecs::resource<gecs::mut<AssetManager>> asset_manager)
    {
        // 处理要交换给renderer的数据
        auto swap_data           = CreateRef<SwapData>();
        swap_data->ui            = ui->ui;
        swap_data->view_matrix   = camera->GetViewMatrix();
        swap_data->view_position = camera->GetPosition();

        // todo 多光源的时候怎么处理
        for (const auto& [_, light] : directional_lights)
        {
            swap_data->directional_light = light;
        }

        for (const auto& [_, mesh, material, transform] : objects)
        {
            swap_data->objects.push_back(RenderObject{mesh, material, transform});
        }

        // 渲染器渲染一帧
        render->renderer->Tick(swap_data);
    }

} // namespace Yutrel