#include "yutrel_pch.hpp"

#include "vulkan_renderer.hpp"

#include "platform/Vulkan/passes/render_pass.hpp"
#include "platform/Vulkan/pipeline/vulkan_pipeline.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include "platform/Vulkan/mesh/mesh.hpp"

namespace Yutrel
{
    void VulkanRenderer::Init(RendererInitInfo info)
    {
        // 初始化RHI
        RHIInitInfo rhi_init_info;
        rhi_init_info.raw_window = info.raw_window;
        rhi_init_info.width      = info.width;
        rhi_init_info.height     = info.height;

        m_rhi = CreateRef<VulkanRHI>();
        m_rhi->Init(rhi_init_info);

        // 初始化pipeline
        RenderPipelineInitInfo pipeline_init_info{};
        m_render_pipeline = CreateRef<VulkanPipeline>();
        m_render_pipeline->SetRHI(m_rhi);
        m_render_pipeline->Init(pipeline_init_info);

        // todo 临时在这里读取
        LoadMesh();
    }

    void VulkanRenderer::Tick()
    {
        // todo 从ECS系统获取渲染对象
        RenderPass::m_triangle_mesh = m_triangle_mesh;

        m_rhi->PrepareContext();

        m_render_pipeline->ForwardRender();
    }

    void VulkanRenderer::Clear()
    {
        // 清除RHI
        if (m_rhi)
        {
            m_rhi->Clear();
        }
        m_rhi.reset();

        // 清除Pipeline
        if (m_render_pipeline)
        {
            m_render_pipeline->Clear();
        }
        m_render_pipeline.reset();
    }

    void VulkanRenderer::LoadMesh()
    {
        m_triangle_mesh = CreateRef<Mesh>();
        m_triangle_mesh->vertices.resize(3);

        m_triangle_mesh->vertices[0].position = {1.0f, 1.0f, 0.0f};
        m_triangle_mesh->vertices[1].position = {-1.0f, 1.0f, 0.0f};
        m_triangle_mesh->vertices[2].position = {0.0f, -1.0f, 0.0f};

        m_triangle_mesh->vertices[0].color = {1.0f, 0.0f, 0.0f};
        m_triangle_mesh->vertices[1].color = {0.0f, 1.0f, 0.0f};
        m_triangle_mesh->vertices[2].color = {0.0f, 0.0f, 1.0f};

        m_rhi->UploadMesh(m_triangle_mesh);
    }

} // namespace Yutrel