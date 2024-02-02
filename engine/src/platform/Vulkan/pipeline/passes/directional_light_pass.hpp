#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vulkan/vulkan_core.h>

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
        VkSampler depth_sampler;

    private:
        //--------初始化---------
        void InitDepthImage();

        void InitPipelines();

        //---------绘制------------
        void Draw();

    private:
        enum pipelines : uint8_t
        {
            main_pipeline = 0,

            pipeline_count,
        };

        struct
        {
            glm::mat4 light_VP;
            glm::mat4 model_matrix;
            VkDeviceAddress vertex_buffer;
        } m_push_constants;

        // 绘制范围
        VkExtent2D m_draw_extent;
    };
} // namespace Yutrel