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

    struct GPUSceneData
    {
        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 view_proj;
        glm::vec4 ambient_color;
        glm::vec4 sunlight_direction; // w for sun power
        glm::vec4 sunlight_color;
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

        void InitTexturePipeline();

        //---------绘制------------
        void PrepareDrawImage();

        void CopyToSwapchain();

        void DrawBackground();

        void DrawGeometry();

    private:
        enum pipelines : uint8_t
        {
            compute_pipeline = 0,
            texture_pipeline,

            pipeline_count,
        };

        enum descriptors : uint8_t
        {
            compute_descriptor = 0,
            scene_descriptor,
            material_descriptor,

            descriptor_count,
        };

        // 绘制范围
        VkExtent2D m_draw_extent;
        // 渲染缩放
        float m_render_scale{1.0f};

        // 绘制到的图像
        AllocatedImage m_draw_image;
        // 深度图像
        AllocatedImage m_depth_image;

        Ref<RenderData> m_render_data;
        // todo 设定场景数据
        GPUSceneData scene_data;
    };
} // namespace Yutrel