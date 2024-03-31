#pragma once

#include "platform/Vulkan/pipeline/passes/render_pass.hpp"

#include <glm/glm.hpp>

#include <stdint.h>

namespace Yutrel
{
    struct BasePassInitInfo : RenderPassInitInfo
    {
    };

    class BasePass final : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

        void Draw();

        vk::DescriptorSetLayout GetMaterialDescriptorSetLayout() const { return m_descriptors[material_descriptor].layout; }

    public:
        // gbuffer
        AllocatedImage gbuffer_base_color;
        AllocatedImage gbuffer_world_normal;
        AllocatedImage gbuffer_world_position;
        AllocatedImage gbuffer_metallic_roughness;
        // 深度图像
        AllocatedImage gbuffer_depth;

    private:
        //--------初始化---------
        void InitDrawImage();

        void InitDepthImage();

        void InitUnifromBuffers();

        void InitDescriptors();

        void InitPipelines();

        //---------绘制------------
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