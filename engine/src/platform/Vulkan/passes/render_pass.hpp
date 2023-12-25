#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/vulkan_types.hpp"
#include <vector>

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

        struct RenderPipelineBase
        {
            VkPipelineLayout layout;
            VkPipeline pipeline;
        };
        std::vector<RenderPipelineBase> m_render_pipelines;
    };
} // namespace Yutrel