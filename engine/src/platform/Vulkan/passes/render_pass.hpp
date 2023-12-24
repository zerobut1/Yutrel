#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    class VulkanRHI;

    struct RenderPassInitInfo
    {};

    class RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) = 0;

        void SetRHI(Ref<VulkanRHI> rhi) { m_rhi = rhi; }

    protected:
        Ref<VulkanRHI> m_rhi;

        VkRenderPass m_render_pass;
    };
} // namespace Yutrel