#include "yutrel_pch.hpp"

#include "renderer.hpp"

#include "core/path/path.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
// #include "platform/OpenGL/opengl_renderer.hpp"

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

    RenderResource::RenderResource()
    {
        m_renderer = CreateRef<VulkanRenderer>();
    }

    RenderResource::~RenderResource()
    {
        m_renderer->Clear();
        LOG_INFO("Renderer Destroyed");
    }

    void RenderResource::Init(gecs::resource<RenderResource> render,
                              gecs::resource<WindowResource> window)
    {
        LOG_INFO("Initialize Render Resource");

        // 初始化renderer
        RendererInitInfo info{};
        info.raw_window = static_cast<GLFWwindow*>(window->GetWindow()->GetRaw());

        render->m_renderer->Init(info);
    }

} // namespace Yutrel