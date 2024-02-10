#include "yutrel_pch.hpp"

#include "directional_light_pass.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"

#include <array>
#include <vector>
#include <vk_mem_alloc_enums.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Yutrel
{
    void DirectionalLightPass::Init(RenderPassInitInfo* info)
    {
        RenderPass::Init(info);

        InitDepthImage();

        InitUnifromBuffers();

        InitDescriptors();

        InitPipelines();
    }

    void DirectionalLightPass::DrawForward()
    {
        m_draw_extent = vk::Extent2D{depth_image.extent.width, depth_image.extent.height};

        UpdateUniformBuffer();

        auto cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        m_rhi->TransitionImage(cmd_buffer,
                               depth_image.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eDepthStencilAttachmentOptimal);

        Draw();

        m_rhi->TransitionImage(cmd_buffer,
                               depth_image.image,
                               vk::ImageLayout::eDepthStencilAttachmentOptimal,
                               vk::ImageLayout::eDepthStencilReadOnlyOptimal);
    }

    void DirectionalLightPass::InitDepthImage()
    {
        //-----------深度图像-----------------
        vk::ImageUsageFlags depth_image_usages;
        depth_image_usages |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        depth_image_usages |= vk::ImageUsageFlagBits::eSampled;

        depth_image = m_rhi->CreateImage(vk::Extent3D{4096, 4096, 1}, vk::Format::eD24UnormS8Uint, depth_image_usages);

        //-----------------采样器--------------
        auto sampler_ci =
            vk::SamplerCreateInfo()
                .setMagFilter(vk::Filter::eLinear)
                .setMinFilter(vk::Filter::eLinear)
                .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                .setAddressModeU(vk::SamplerAddressMode::eClampToBorder)
                .setAddressModeV(vk::SamplerAddressMode::eClampToBorder)
                .setAddressModeW(vk::SamplerAddressMode::eClampToBorder)
                .setMipLodBias(0.0f)
                .setMinLod(0.0f)
                .setMaxLod(1.0f)
                .setBorderColor(vk::BorderColor::eIntOpaqueWhite)
                .setAnisotropyEnable(vk::True)
                .setMaxAnisotropy(m_rhi->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy)
                .setUnnormalizedCoordinates(vk::False)
                .setCompareEnable(vk::False)
                .setCompareOp(vk::CompareOp::eAlways);

        depth_sampler = m_rhi->CreateSampler(sampler_ci);
    }

    void DirectionalLightPass::InitUnifromBuffers()
    {
        m_scene_uniform_buffer =
            m_rhi->CreateBuffer(sizeof(m_scene_uniform_data),
                                vk::BufferUsageFlagBits::eUniformBuffer,
                                vma::MemoryUsage::eCpuToGpu);
    }

    void DirectionalLightPass::InitDescriptors()
    {
        m_descriptors.resize(descriptor_count);

        // 场景信息
        {
            // 创建描述符布局
            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eUniformBuffer)
                    .SetShaderStage(vk::ShaderStageFlagBits::eVertex);

            m_descriptors[scene_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            // 分配描述符集
            m_descriptors[scene_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[scene_descriptor].layout);

            // 写描述符集
            auto writer =
                DescriptorWriter()
                    .WriteBuffer(0, m_scene_uniform_buffer.buffer, sizeof(m_scene_uniform_data), 0, vk::DescriptorType::eUniformBuffer);

            m_rhi->UpdateDescriptorSets(writer, m_descriptors[scene_descriptor].set);
        }
    }

    void DirectionalLightPass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);
        {
            //----------shader--------------
            vk::ShaderModule shadowmap_vert_shader = m_rhi->CreateShaderModule(SHADOWMAP_VERT_CODE);
            vk::ShaderModule shadowmap_frag_shader = m_rhi->CreateShaderModule(SHADOWMAP_FRAG_CODE);

            //------------推送常量------------
            auto push_constant_range =
                vk::PushConstantRange()
                    .setOffset(0)
                    .setSize(sizeof(m_push_constants))
                    .setStageFlags(vk::ShaderStageFlagBits::eVertex);

            //-----------管线布局-------------
            std::vector<vk::DescriptorSetLayout> descriptor_set_layouts{
                m_descriptors[scene_descriptor].layout,
            };

            auto pipeline_ci =
                vk::PipelineLayoutCreateInfo()
                    .setFlags({})
                    .setPushConstantRanges(push_constant_range)
                    .setSetLayouts(descriptor_set_layouts);
            m_pipelines[main_pipeline].layout = m_rhi->CreatePipelineLayout(pipeline_ci);

            //-----------创建管线----------
            auto render_pipeline_ci =
                RenderPipelineCreateInfo()
                    .SetPipelineLayout(m_pipelines[main_pipeline].layout)
                    .SetShaders(shadowmap_vert_shader, shadowmap_frag_shader)
                    .SetInputTopolygy(vk::PrimitiveTopology::eTriangleList)
                    .SetPolygonMode(vk::PolygonMode::eFill)
                    .SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise)
                    .SetMultisamplingNone()
                    .DisableBlending()
                    .EnableDepthTest(vk::True, vk::CompareOp::eGreaterOrEqual)
                    .SetDepthFormat(depth_image.format);

            m_pipelines[main_pipeline].pipeline = m_rhi->CreateRenderPipeline(render_pipeline_ci);

            //--------清除---------
            m_rhi->DestroyShaderModule(shadowmap_vert_shader);
            m_rhi->DestroyShaderModule(shadowmap_frag_shader);
        }
    }

    void DirectionalLightPass::UpdateUniformBuffer()
    {
        m_scene_uniform_data.light_VP = m_render_scene->directional_light_VP;

        memcpy(m_scene_uniform_buffer.info.pMappedData, &m_scene_uniform_data, sizeof(m_scene_uniform_data));
    }

    void DirectionalLightPass::Draw()
    {
        vk::CommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        //---------渲染信息----------------
        // 清除信息
        auto clear_value =
            vk::ClearValue()
                .setDepthStencil(vk::ClearDepthStencilValue(0.0f));

        // 深度附件
        auto depth_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(depth_image.image_view)
                .setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setClearValue(clear_value);

        // 开始渲染
        auto render_info =
            vk::RenderingInfo()
                .setRenderArea(vk::Rect2D({0, 0}, m_draw_extent))
                .setLayerCount(1)
                .setPDepthAttachment(&depth_attachment);

        cmd_buffer.beginRendering(render_info);

        // 绑定管线
        cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].pipeline);

        // 设定viewport
        auto viewport =
            vk::Viewport()
                .setX(0)
                .setY(0)
                .setWidth(m_draw_extent.width)
                .setHeight(m_draw_extent.height)
                .setMinDepth(0.0f)
                .setMaxDepth(1.0f);

        cmd_buffer.setViewport(0, viewport);

        // 设定scissor
        cmd_buffer.setScissor(0, vk::Rect2D({0, 0}, m_draw_extent));

        // 绑定全局变量描述符
        cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].layout, 0, m_descriptors[scene_descriptor].set, {});

        for (auto& entity : m_render_scene->render_entities)
        {
            m_push_constants.model_matrix  = entity.model_matrix;
            m_push_constants.vertex_buffer = entity.mesh->vertex_buffer_address;

            cmd_buffer.pushConstants(m_pipelines[main_pipeline].layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(m_push_constants), &m_push_constants);

            // 绑定IBO
            cmd_buffer.bindIndexBuffer(entity.mesh->index_buffer.buffer, 0, vk::IndexType::eUint32);

            cmd_buffer.drawIndexed(entity.mesh->index_count, 1, 0, 0, 0);
        }
        cmd_buffer.endRendering();
    }

} // namespace Yutrel