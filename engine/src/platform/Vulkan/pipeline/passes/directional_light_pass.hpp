#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>
#include <vulkan/vulkan_handles.hpp>

namespace Yutrel
{
    struct DirectionalLightPassInitInfo : RenderPassInitInfo
    {};

    class DirectionalLightPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        void DrawForward();

    public:
        // 深度图像
        AllocatedImage depth_image;
        vk::Sampler depth_sampler;

    private:
        //--------初始化---------
        void InitDepthImage();

        void InitUnifromBuffers();

        void InitDescriptors();

        void InitPipelines();

        //---------绘制-----------
        void UpdateUniformBuffer();

        void Draw();

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

        struct
        {
            // todo 控制array大小
            std::array<glm::mat4, 4> light_VP;
        } m_scene_uniform_data;
        AllocatedBuffer m_scene_uniform_buffer;

        struct
        {
            glm::mat4 model_matrix;
            VkDeviceAddress vertex_buffer;
            uint32_t cascade_index;
        } m_push_constants;

        // 绘制范围
        vk::Extent2D m_draw_extent;

        // 级联阴影贴图的图像视图
        std::vector<vk::ImageView> m_depth_image_views;
    };
} // namespace Yutrel