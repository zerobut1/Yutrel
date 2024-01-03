#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>

namespace Yutrel
{
    struct TestPassInitInfo : RenderPassInitInfo
    {};

    struct ComputePushConstants
    {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

    struct GPUDrawPushConstants
    {
        glm::mat4 world_matrix;
        VkDeviceAddress vertex_buffer;
    };

    class TestPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        virtual void PreparePassData(Ref<struct RenderData> render_data) override;

        void DrawForward();

    private:
        //--------初始化---------
        void InitDrawImage();

        void InitDepthImage();

        void InitDescriptors();

        void InitComputePipeline();

        void InitTrianglePipeline();

        //---------绘制------------
        void PrepareDrawImage();

        void CopyToSwapchain();

        void DrawBackground();

        void DrawGeometry();

    private:
        enum pipelines : uint8_t
        {
            compute = 0,
            triangle,

            count,
        };

        // 绘制范围
        VkExtent2D m_draw_extent;
        // 渲染缩放
        float m_render_scale{1.0f};

        // 绘制到的图像
        AllocatedImage m_draw_image;
        // 深度图像
        AllocatedImage m_depth_image;

        Ref<struct RenderData> m_render_data;
    };
} // namespace Yutrel