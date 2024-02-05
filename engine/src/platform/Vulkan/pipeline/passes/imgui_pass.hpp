#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

namespace Yutrel
{
    struct ImguiPassInitInfo : RenderPassInitInfo
    {};

    class ImguiPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;


        void DrawImgui();
    };
} // namespace Yutrel