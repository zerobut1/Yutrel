#include "yutrel_pch.hpp"

#include "renderer.hpp"

#include "core/application/application.hpp"
#include "core/path/path.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include "resource/asset/asset.hpp"
#include "resource/component/component.hpp"

// #include "platform/OpenGL/opengl_renderer.hpp"

#include <GLFW/glfw3.h>
#include <vector>

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
                                  gecs::resource<gecs::mut<AssetManager>> asset_manager)
    {
        auto render_data = CreateRef<RenderData>();
        for (auto&& [entity, pbr_bundle] : pbrs)
        {
            if (!pbr_bundle.mesh.is_loaded)
            {
                if (!asset_manager->LoadFromFile(pbr_bundle.mesh))
                {
                    LOG_ERROR("Failed to load {}", pbr_bundle.mesh.path);
                }
            }

            render_data->pbrs.push_back(&pbr_bundle);
        }
        render->renderer->Tick(render_data);
    }

} // namespace Yutrel