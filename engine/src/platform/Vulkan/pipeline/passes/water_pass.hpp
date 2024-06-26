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
        void InitImages();

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
            color_descriptor,

            descriptor_count,
        };

        // 绘制范围
        vk::Extent2D m_draw_extent;
        // 绘制到的图像
        AllocatedImage m_draw_image;
        // 深度图像
        AllocatedImage m_depth_image;

        AllocatedImage m_scene_color;
        AllocatedImage m_scene_depth;

        struct
        {
            glm::mat4 view;
            glm::mat4 projection;
            glm::vec3 camera_position;
            float padding_1;
            // 光源颜色的第四位为intensity
            glm::vec4 directional_light_color;
            glm::vec3 directional_light_direction;
            float near_plane;
            float far_plane;
        } m_scene_uniform_data;
        AllocatedBuffer m_scene_uniform_buffer;

        struct
        {
            glm::mat4 model_matrix;
            vk::DeviceAddress vertex_buffer;
            float width;
            float height;
            float time;
        } m_push_constants;
    };

} // namespace Yutrel