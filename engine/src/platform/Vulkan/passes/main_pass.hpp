#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"

namespace Yutrel
{
    struct MainPassInitInfo : RenderPassInitInfo
    {};

    class MainPass : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

    private:
        void InitRenderPass();
        void InitFramebuffer();
        void InitPipeline();

    private:
        std::vector<VkFramebuffer> m_swapchain_framebuffers;
    };
} // namespace Yutrel