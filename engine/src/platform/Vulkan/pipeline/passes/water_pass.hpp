#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <array>
#include <glm/glm.hpp>

#include <stdint.h>

namespace Yutrel
{
    struct WaterPassInitInfo : RenderPassInitInfo
    {
        AllocatedImage draw_image;
        AllocatedImage depth_image;
    };

    class WaterPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        void Draw();

    private:
        //--------初始化---------
        void InitUnifromBuffers();

        void InitDescriptors();

        void InitPipelines();

        //---------绘制------------
        void CopyToSwapchain();

        void UpdateUniformBuffer();

        void DrawGeometry();

    private:
        enum pipelines : uint8_t
        {
            main_pipeline = 0,

            pipeline_count,
        };

        enum descriptors : uint8_t
        {
            scene_descriptor = 0,

            descriptor_count,
        };

        // 绘制范围
        vk::Extent2D m_draw_extent;
        // 绘制到的图像
        AllocatedImage m_draw_image;
        // 深度图像
        AllocatedImage m_depth_image;

        struct
        {
            glm::mat4 view;
            glm::mat4 projection;
        } m_scene_uniform_data;
        AllocatedBuffer m_scene_uniform_buffer;

        struct
        {
            glm::mat4 model_matrix;
            vk::DeviceAddress vertex_buffer;
        } m_push_constants;
    };
} // namespace Yutrel