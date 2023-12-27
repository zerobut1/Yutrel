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

        virtual void Tick(Ref<RenderData> render_data) override;

        virtual void Clear() override;

    private:
        void ProcessRenderData();

    private:
        Ref<VulkanRHI> m_rhi;
        Ref<VulkanPipeline> m_render_pipeline;

        Ref<RenderData> m_render_data;
    };
} // namespace Yutrel