#include "yutrel_pch.hpp"

#include "renderer.hpp"

#include "core/application/application.hpp"
#include "core/path/path.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include "resource/asset/asset.hpp"
#include "resource/asset/material.hpp"
#include "resource/asset/mesh.hpp"
#include "resource/asset/texture.hpp"
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

    void RendererResource::Update(gecs::querier<PbrBundle> pbrs,
                                  gecs::resource<RendererResource> render,
                                  gecs::resource<UIResource> ui,
                                  gecs::resource<BackGroundColor> background_color,
                                  gecs::resource<gecs::mut<AssetManager>> asset_manager)
    {
        // 处理要交换给renderer的数据
        auto swap_data        = CreateRef<SwapData>();
        swap_data->ui         = ui->ui;
        swap_data->background = background_color.get();

        for (const auto& [entity, pbr_bundle] : pbrs)
        {
            // 加载模型
            if (!pbr_bundle.mesh->is_loaded)
            {
                asset_manager->LoadFromFile(pbr_bundle.mesh);
            }

            // 加载基础色纹理
            if (!pbr_bundle.material->base_color_texture->is_loaded)
            {
                asset_manager->LoadFromFile(pbr_bundle.material->base_color_texture);
            }

            swap_data->pbrs.push_back(&pbr_bundle);
        }

        // 渲染器渲染一帧
        render->renderer->Tick(swap_data);
    }

} // namespace Yutrel