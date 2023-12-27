#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"

#include <stdint.h>

namespace Yutrel
{
    struct MainPassInitInfo : RenderPassInitInfo
    {};

    class MainPass : public RenderPass
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
        std::vector<VkFramebuffer> m_swapchain_framebuffers;

        Ref<struct RenderData> m_render_data;
    };
} // namespace Yutrel