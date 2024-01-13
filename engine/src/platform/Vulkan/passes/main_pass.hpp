#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    struct MainPassInitInfo : RenderPassInitInfo
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
        glm::mat4 model_matrix;
        VkDeviceAddress vertex_buffer;
    };

    class MainPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        virtual void PreparePassData(Ref<struct RenderData> render_data) override;

        RendererStatus DrawForward();

        VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const { return m_descriptor_infos[material_descriptor].layout; }

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

        RendererStatus DrawGeometry();

    private:
        enum pipelines : uint8_t
        {
            compute_pipeline = 0,
            texture_pipeline,

            pipeline_count,
        };

        enum descriptors : uint8_t
        {
            draw_image_descriptor = 0,
            scene_descriptor,
            material_descriptor,

            descriptor_count,
        };

        // 绘制范围
        VkExtent2D m_draw_extent;

        // 绘制到的图像
        AllocatedImage m_draw_image;
        // 深度图像
        AllocatedImage m_depth_image;

        Ref<RenderData> m_render_data;
    };
} // namespace Yutrel