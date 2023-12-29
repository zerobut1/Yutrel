#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <vector>

namespace Yutrel
{
    class VulkanRHI;
    class Mesh;

    struct RenderPassInitInfo
    {};

    class RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) = 0;

        virtual void PreparePassData(Ref<struct RenderData> render_data) = 0;

        void SetRHI(Ref<VulkanRHI> rhi) { m_rhi = rhi; }

    protected:
        Ref<VulkanRHI> m_rhi;

        struct PipelineBase
        {
            VkPipelineLayout layout;
            VkPipeline pipeline;
        };
        std::vector<PipelineBase> m_pipelines;

        struct Descriptor
        {
            VkDescriptorSetLayout layout;
            VkDescriptorSet set;
        };
        std::vector<Descriptor> m_descriptor_infos;
    };
} // namespace Yutrel