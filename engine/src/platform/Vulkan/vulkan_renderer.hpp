#pragma once

#include "function/render/renderer.hpp"

namespace Yutrel
{
    class VulkanRHI;
    class VulkanPipeline;
    struct Mesh;

    class VulkanRenderer : public Renderer
    {
    public:
        virtual void Init(RendererInitInfo info) override;

        virtual void Tick() override;

        virtual void Clear() override;

    private:
        // todo 临时
        void LoadMesh();

    private:
        Ref<VulkanRHI> m_rhi;
        Ref<VulkanPipeline> m_render_pipeline;

        // todo 临时
        Ref<Mesh> m_triangle_mesh;
    };
} // namespace Yutrel