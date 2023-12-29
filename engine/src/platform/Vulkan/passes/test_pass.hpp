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
        //--------初始化---------
        void InitDrawImage();

        void InitDescriptors();

        void InitBackgroundPipeline();

        //---------绘制------------
        void PrepareDrawImage();

        void CopyToSwapchain();

        void DrawBackground();

    private:
        // 绘制到的图像
        AllocatedImage m_draw_image;
        VkExtent2D m_draw_extent;

        Ref<struct RenderData> m_render_data;
    };
} // namespace Yutrel