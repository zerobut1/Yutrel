#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    struct MainPassInitInfo : RenderPassInitInfo
    {
        AllocatedImage directional_light_shadowmap;
        VkSampler shadowmap_sampler;
    };

    class MainPass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        void DrawForward();

        VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const { return m_descriptor_infos[material_descriptor].layout; }

    private:
        //--------初始化---------
        void InitDrawImage();

        void InitDepthImage();

        void InitDescriptors();

        void InitPipelines();

        //---------绘制------------
        void PrepareDrawImage();

        void CopyToSwapchain();

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

        struct
        {
            glm::mat4 directional_light_VP;
            glm::mat4 model_matrix;
            VkDeviceAddress vertex_buffer;
        } m_push_constants;

        // 绘制范围
        VkExtent2D m_draw_extent;
        // 绘制到的图像
        AllocatedImage m_draw_image;
        // 深度图像
        AllocatedImage m_depth_image;

        // 平行光shadowmap
        AllocatedImage m_directional_light_shadowmap;
        VkSampler m_shadowmap_sampler;
    };
} // namespace Yutrel