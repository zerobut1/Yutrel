#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    struct DebugDrawPassInitInfo : RenderPassInitInfo
    {
        AllocatedImage image_to_draw;
    };

    class DebugDrawPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        virtual void PreparePassData(Ref<struct RenderData> render_data) override;

        void DrawForward();

    private:
        //--------初始化---------
        void InitDrawImage();

        void InitDescriptors();

        void InitPipelines();

        //---------绘制------------
        void Draw();

    private:
        enum pipelines : uint8_t
        {
            main_pipeline = 0,

            pipeline_count,
        };

        enum descriptors : uint8_t
        {
            image_descriptor = 0,

            descriptor_count,
        };

        // 绘制范围
        VkExtent2D m_draw_extent;

        // 绘制图像
        AllocatedImage m_draw_image;

        // 要绘制的图像
        AllocatedImage m_image_to_draw;
        VkSampler m_sampler;

        Ref<RenderData> m_render_data;
    };
} // namespace Yutrel