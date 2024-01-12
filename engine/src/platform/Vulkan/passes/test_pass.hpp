#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"

#include <stdint.h>

namespace Yutrel
{
    struct TestPassInitInfo : RenderPassInitInfo
    {};

    class TestPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        virtual void PreparePassData(Ref<struct RenderData> render_data) override;

        void DrawForward();

    private:
        void InitRenderPass();
        void InitFramebuffer();
        void InitPipeline();

    private:
        VkRenderPass m_render_pass;

        std::vector<VkFramebuffer> m_swapchain_framebuffers;

        Ref<RenderData> m_render_data;
    };
} // namespace Yutrel