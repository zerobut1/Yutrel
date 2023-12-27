#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <vector>

namespace Yutrel
{
    class VulkanRHI;
    struct Mesh;

    struct RenderPassInitInfo
    {};

    class RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) = 0;

        virtual void PreparePassData(Ref<struct RenderData> render_data) = 0;

        void SetRHI(Ref<VulkanRHI> rhi) { m_rhi = rhi; }

    public:
        // todo 临时
        static inline Ref<Mesh> m_triangle_mesh;

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