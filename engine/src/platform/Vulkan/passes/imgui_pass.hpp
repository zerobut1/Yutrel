#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"

namespace Yutrel
{
    struct ImguiPassInitInfo : RenderPassInitInfo
    {};

    class ImguiPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        virtual void PreparePassData(Ref<struct RenderData> render_data) override;

        void DrawImgui();

    private:
        Ref<struct RenderData> m_render_data;
    };
} // namespace Yutrel