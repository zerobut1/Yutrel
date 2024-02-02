#include "yutrel_pch.hpp"

#include "debug_draw_pass.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    void DebugDrawPass::Init(RenderPassInitInfo* info)
    {
        RenderPass::Init(info);
        m_image_to_draw = static_cast<DebugDrawPassInitInfo*>(info)->image_to_draw;

        InitDrawImage();

        InitDescriptors();

        InitPipelines();
    }

    void DebugDrawPass::DrawForward()
    {
        m_draw_extent   = {m_draw_image.image_extent.width, m_draw_image.image_extent.height};
        auto cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        m_rhi->TransitionImage(cmd_buffer,
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        Draw();
    }

    void DebugDrawPass::InitDrawImage()
    {
        m_draw_image.image_format = VK_FORMAT_R8G8B8A8_SNORM;
        m_draw_image.image_extent = m_image_to_draw.image_extent;

        VkImageUsageFlags draw_image_usages{};
        draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
        draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo draw_image_create_info = vkinit::ImageCreateInfo(m_draw_image.image_format, draw_image_usages, m_draw_image.image_extent);

        // 将图像放至GPU only内存
        VmaAllocationCreateInfo draw_image_alloc_info{};
        draw_image_alloc_info.usage         = VMA_MEMORY_USAGE_GPU_ONLY;
        draw_image_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_rhi->CreateDrawImage(&draw_image_create_info, &draw_image_alloc_info, &m_draw_image);

        // 创建图像视图
        VkImageViewCreateInfo draw_image_view_info = vkinit::ImageViewCreateInfo(m_draw_image.image_format, m_draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);

        m_rhi->CreateDrawImageView(&draw_image_view_info, &m_draw_image);

        //-----------------采样器--------------
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter     = VK_FILTER_LINEAR;
        sampler_info.minFilter     = VK_FILTER_LINEAR;
        sampler_info.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.addressModeU  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeV  = sampler_info.addressModeU;
        sampler_info.addressModeW  = sampler_info.addressModeU;
        sampler_info.mipLodBias    = 0.0f;
        sampler_info.maxAnisotropy = 1.0f;
        sampler_info.minLod        = 0.0f;
        sampler_info.maxLod        = 1.0f;
        sampler_info.borderColor   = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        m_rhi->CreateSampler(&sampler_info, &m_sampler);
    }

    void DebugDrawPass::InitDescriptors()
    {
        m_descriptor_infos.resize(descriptor_count);

        // 绘制图片信息
        {
            // 创建描述符布局
            DescriptorSetLayoutCreateInfo layout_info{};
            layout_info.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            layout_info.shader_stages = VK_SHADER_STAGE_FRAGMENT_BIT;

            m_rhi->CreateDescriptorLayout(layout_info, &m_descriptor_infos[image_descriptor].layout);

            // 分配描述符集
            m_rhi->AllocateDescriptorSets(m_descriptor_infos[image_descriptor].layout, &m_descriptor_infos[image_descriptor].set);

            // 写描述符集
            DescriptorWriter writer;
            writer.WriteImage(0, m_image_to_draw.image_view, m_sampler, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            m_rhi->UpdateDescriptorSets(writer, m_descriptor_infos[image_descriptor].set);
        }
    }

    void DebugDrawPass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);
        {
            //----------shader--------------
            VkShaderModule quad_vert_shader;
            if (!m_rhi->CreateShaderModule(QUAD_VERT_CODE, &quad_vert_shader))
            {
                LOG_ERROR("Failed to create vertex shader");
            }
            VkShaderModule quad_frag_shader;
            if (!m_rhi->CreateShaderModule(QUAD_FRAG_CODE, &quad_frag_shader))
            {
                LOG_ERROR("Failed to create fragment shader");
            }

            //-----------管线布局-------------
            std::array<VkDescriptorSetLayout, 1> descriptor_set_layouts{
                m_descriptor_infos[image_descriptor].layout,
            };

            VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::PipelineLayoutCreateInfo();
            pipeline_layout_info.setLayoutCount             = descriptor_set_layouts.size();
            pipeline_layout_info.pSetLayouts                = descriptor_set_layouts.data();
            m_rhi->CreatePipelineLayout(&pipeline_layout_info, &m_pipelines[main_pipeline].layout);

            //-----------创建管线----------
            DynamicPipelineCreateInfo pipeline_create_info;
            pipeline_create_info.Clear();
            pipeline_create_info.pipeline_layout = m_pipelines[main_pipeline].layout;
            pipeline_create_info.SetShaders(quad_vert_shader, quad_frag_shader);
            pipeline_create_info.SetInputTopolygy(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
            pipeline_create_info.SetPolygonMode(VK_POLYGON_MODE_FILL);
            pipeline_create_info.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            pipeline_create_info.SetMultisamplingNone();
            pipeline_create_info.DisableBlending();
            pipeline_create_info.DisableDepthTest();

            m_rhi->CreateDynamicPipelines(pipeline_create_info, &m_pipelines[main_pipeline].pipeline);

            //--------清除---------
            m_rhi->DestroyShaderModule(quad_vert_shader);
            m_rhi->DestroyShaderModule(quad_frag_shader);
        }
    }

    void DebugDrawPass::Draw()
    {
        VkCommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        //---------渲染信息----------------
        // 清除信息
        VkClearValue clear_value{};
        clear_value.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

        // 颜色附件
        VkRenderingAttachmentInfo color_attachment = vkinit::AttachmentInfo(m_draw_image.image_view, &clear_value, VK_IMAGE_LAYOUT_GENERAL);
        // 深度附件
        VkRenderingAttachmentInfo depth_attachment = vkinit::DepthAttachmentInfo(VK_NULL_HANDLE, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        // 开始渲染
        VkRenderingInfo render_info = vkinit::RenderingInfo(m_draw_extent, &color_attachment, &depth_attachment);
        vkCmdBeginRendering(cmd_buffer, &render_info);

        // 绑定管线
        vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[pipelines::main_pipeline].pipeline);

        // 设定viewport
        VkViewport viewport{};
        viewport.x        = 0;
        viewport.y        = 0;
        viewport.width    = m_draw_extent.width;
        viewport.height   = m_draw_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);

        // 设定scissor
        VkRect2D scissor{};
        scissor.offset.x      = 0;
        scissor.offset.y      = 0;
        scissor.extent.width  = m_draw_extent.width;
        scissor.extent.height = m_draw_extent.height;
        vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);

        // vkCmdBindDescriptorSets(cmd_buffer)
        vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[main_pipeline].layout, 0, 1, &m_descriptor_infos[image_descriptor].set, 0, nullptr);

        vkCmdDraw(cmd_buffer, 3, 1, 0, 0);

        vkCmdEndRendering(cmd_buffer);
    }

} // namespace Yutrel