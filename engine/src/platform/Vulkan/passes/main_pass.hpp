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

    private:
    };
} // namespace Yutrel