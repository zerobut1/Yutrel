#pragma once

#include "core/macro.hpp"

namespace Yutrel
{
    class VulkanRHI;
    class RenderPass;

    struct RenderPipelineInitInfo
    {
    };

    class VulkanPipeline
    {
    public:
        void Init(RenderPipelineInitInfo info);

        void Clear();

        void SetRHI(Ref<VulkanRHI> rhi) { m_rhi = rhi; }

    private:
        Ref<VulkanRHI> m_rhi;

        Ref<RenderPass> m_main_pass;
    };
} // namespace Yutrel