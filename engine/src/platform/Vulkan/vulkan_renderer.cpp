#include "yutrel_pch.hpp"

#include "vulkan_renderer.hpp"

#include "platform/Vulkan/pipeline/vulkan_pipeline.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

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
    }

    void VulkanRenderer::Tick()
    {
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

} // namespace Yutrel