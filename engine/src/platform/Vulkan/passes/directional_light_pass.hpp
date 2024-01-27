#pragma once

#include "platform/Vulkan/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    struct DirectionalLightPassInitInfo : RenderPassInitInfo
    {};

    struct PushConstants
    {
        glm::mat4 light_MVP;
        VkDeviceAddress vertex_buffer;
    };

    class DirectionalLightPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        virtual void PreparePassData(Ref<struct RenderData> render_data) override;

        void DrawForward();

    public:
        // 深度图像
        AllocatedImage m_depth_image;
        VkSampler m_depth_sampler;

    private:
        //--------初始化---------
        void InitDrawImage();

        void InitDepthImage();

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
            light_descriptor = 0,

            descriptor_count,
        };

        // 绘制范围
        VkExtent2D m_draw_extent;

        // 绘制图像
        AllocatedImage m_draw_image;

        Ref<RenderData> m_render_data;
    };
} // namespace Yutrel