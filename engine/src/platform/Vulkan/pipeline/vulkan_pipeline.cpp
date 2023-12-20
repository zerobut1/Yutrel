#include "yutrel_pch.hpp"

#include "vulkan_pipeline.hpp"

#include "platform/Vulkan/passes/main_pass.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

namespace Yutrel
{
    void VulkanPipeline::Init(RenderPipelineInitInfo info)
    {
        m_main_pass = CreateRef<MainPass>();

        m_main_pass->SetRHI(m_rhi);

        MainPassInitInfo main_init_info{};
        m_main_pass->Init(&main_init_info);
    }

    void VulkanPipeline::Clear()
    {
        
    }
} // namespace Yutrel