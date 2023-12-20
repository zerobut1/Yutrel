#pragma once

#include "function/render/renderer.hpp"

namespace Yutrel
{
    class RHI;
    class RenderPipeline;

    class VulkanRenderer : public Renderer
    {
    public:
        virtual void Init(RendererInitInfo info) override;

        virtual void Clear() override;

    private:
        Ref<RHI> m_rhi;
        Ref<RenderPipeline> m_render_pipeline;
    };
} // namespace Yutrel