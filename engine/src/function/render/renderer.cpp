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
    // Ref<RenderData> Renderer::m_render_data = CreateRef<RenderData>();

    // void Renderer::InitRenderData(Yutrel::Commands& cmd, Yutrel::Resources resources)
    // {
    //     // 设置全局通用的数据
    //     auto paths = resources.Get<Paths>();

    //     m_render_data->plane_model        = Model::Create(paths.resource + "model\\basic\\plane\\plane.obj", false);
    //     m_render_data->cube_model         = Model::Create(paths.resource + "model\\basic\\cube\\cube.obj", false);
    //     m_render_data->sphere_model       = Model::Create(paths.resource + "model\\basic\\sphere\\sphere.obj", false);
    //     m_render_data->framebuffer_shader = Shader::Create(paths.asset + "shader\\core\\framebuffer.vert", paths.asset + "shader\\core\\framebuffer.frag");
    //     m_render_data->skybox_shader      = Shader::Create(paths.asset + "shader\\core\\skybox.vert", paths.asset + "shader\\core\\skybox.frag");
    //     m_render_data->irradiance_shader  = ComputeShader::Create(paths.asset + "shader\\core\\irradiance_map.glsl");
    //     m_render_data->prefilter_shader   = ComputeShader::Create(paths.asset + "shader\\core\\prefilter_map.glsl");
    //     m_render_data->brdf_LUT           = Texture::Create(paths.resource + "texture\\pbr\\brdf_lut.hdr", true);
    // }

    RendererResource::RendererResource()
    {
        renderer = CreateRef<VulkanRenderer>();
    }

    RendererResource::~RendererResource()
    {
        renderer->Clear();
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
        render->renderer->Tick(swap_data);
    }

} // namespace Yutrel