#pragma once

#include "function/render/renderer.hpp"

namespace Yutrel
{
    class VulkanRHI;
    class VulkanPipeline;

    class VulkanRenderer : public Renderer
    {
    public:
        virtual void Init(RendererInitInfo info) override;

        virtual void Clear() override;

    private:
        Ref<VulkanRHI> m_rhi;
        Ref<VulkanPipeline> m_render_pipeline;
    };
} // namespace Yutrel