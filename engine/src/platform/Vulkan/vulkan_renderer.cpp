#include "yutrel_pch.hpp"

#include "vulkan_renderer.hpp"

#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

namespace Yutrel
{
    void VulkanRenderer::Init(RendererInitInfo info)
    {
        // 初始化RHI
        RHIInitInfo rhi_init_info;
        rhi_init_info.raw_window = info.raw_window;

        m_rhi = CreateRef<VulkanRHI>();
        m_rhi->Init(rhi_init_info);

        // todo 初始化pipeline
    }

    void VulkanRenderer::Clear()
    {
        // 清除RHI
        if (m_rhi)
        {
            m_rhi->Clear();
        }
        m_rhi.reset();
    }

} // namespace Yutrel