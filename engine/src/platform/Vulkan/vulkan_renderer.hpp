#pragma once

#include "function/render/renderer.hpp"

namespace Yutrel
{
    class VulkanRHI;
    class VulkanPipeline;

    class VulkanRenderer final : public Renderer
    {
    public:
        virtual void Init(RendererInitInfo info) override;

        virtual void Tick(Ref<RenderData> render_data) override;

        virtual void Clear() override;

        virtual void UpdateWindowSize(uint32_t width, uint32_t height) override;

    private:
        void ProcessRenderData();

        void NewImguiFrame();

    private:
        Ref<VulkanRHI> m_rhi;
        Ref<VulkanPipeline> m_render_pipeline;

        Ref<RenderData> m_render_data;
    };
} // namespace Yutrel