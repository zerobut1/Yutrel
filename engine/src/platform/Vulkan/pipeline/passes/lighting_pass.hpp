#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <array>
#include <glm/glm.hpp>

#include <stdint.h>

namespace Yutrel
{
    struct LightingPassInitInfo : RenderPassInitInfo
    {
        AllocatedImage gbuffer_base_color;
        AllocatedImage gbuffer_world_normal;
        AllocatedImage gbuffer_world_position;
        AllocatedImage gbuffer_metallic_roughness;
        AllocatedImage gbuffer_depth;

        AllocatedImage directional_light_shadowmap;
        vk::Sampler shadowmap_sampler;
    };

    class LightingPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        void Draw();

    public:
        // 绘制到的图像
        AllocatedImage draw_image;
        // 深度图像
        AllocatedImage depth_image;

    private:
        //--------初始化---------
        void InitGbuffer(LightingPassInitInfo* info);

        void InitDrawImage();

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
            skybox_pipeline,

            pipeline_count,
        };

        enum descriptors : uint8_t
        {
            gbuffer_descriptor = 0,
            scene_descriptor,
            skybox_descriptor,

            descriptor_count,
        };

        // 绘制范围
        vk::Extent2D m_draw_extent;

        AllocatedImage gbuffer_base_color;
        AllocatedImage gbuffer_world_normal;
        AllocatedImage gbuffer_world_position;
        AllocatedImage gbuffer_metallic_roughness;

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
            vk::DeviceAddress vertex_buffer;
        } m_skybox_push_constants;

        struct
        {
            float width;
            float height;
        } m_push_constants;
    };
} // namespace Yutrel