#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <array>
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
            glm::mat4 view;
            glm::mat4 projection;
            glm::vec3 camera_position;
            float padding_1;
            // 光源颜色的第四位为intensity
            glm::vec4 ambient_color;
            glm::vec4 directional_light_color;
            glm::vec3 directional_light_direction;
            float padding_2;
            // alignas(64) glm::mat4 directional_light_VP;
            // todo 控制级联阴影大小
            std::array<float, 4> cascade_splits;
            std::array<glm::mat4, 4> directional_light_VP;
        } m_scene_uniform_data;
        AllocatedBuffer m_scene_uniform_buffer;

        struct
        {
            glm::mat4 model_matrix;
            vk::DeviceAddress vertex_buffer;
        } m_push_constants;
    };
} // namespace Yutrel