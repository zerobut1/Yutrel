#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>

namespace Yutrel
{
    struct MainPassInitInfo : RenderPassInitInfo
    {
        AllocatedImage directional_light_shadowmap;
        vk::Sampler shadowmap_sampler;
    };

    class MainPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        void DrawForward();

        vk::DescriptorSetLayout GetMaterialDescriptorSetLayout() const { return m_descriptors[material_descriptor].layout; }

    private:
        //--------初始化---------
        void InitDrawImage();

        void InitDepthImage();

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
            material_descriptor,

            descriptor_count,
        };

        // 绘制范围
        vk::Extent2D m_draw_extent;
        // 绘制到的图像
        AllocatedImage m_draw_image;
        // 深度图像
        AllocatedImage m_depth_image;

        // 平行光shadowmap
        AllocatedImage m_directional_light_shadowmap;
        vk::Sampler m_shadowmap_sampler;

        struct
        {
            alignas(64) glm::mat4 view;
            alignas(64) glm::mat4 projection;
            alignas(16) glm::vec3 camera_position;
            // 光源颜色的第四位为intensity
            alignas(16) glm::vec4 ambient_color;
            alignas(16) glm::vec4 directional_light_color;
            alignas(16) glm::vec3 directional_light_direction;
            alignas(64) glm::mat4 directional_light_VP;
        } m_scene_uniform_data;
        AllocatedBuffer m_scene_uniform_buffer;

        struct
        {
            glm::mat4 model_matrix;
            vk::DeviceAddress vertex_buffer;
        } m_push_constants;
    };
} // namespace Yutrel