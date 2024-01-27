#include "yutrel_pch.hpp"

#include "directional_light_pass.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"

#include <array>
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    void DirectionalLightPass::Init(RenderPassInitInfo* info)
    {
        m_global_render_data = info->global_render_data;

        InitDrawImage();

        InitDepthImage();

        InitDescriptors();

        InitPipelines();
    }

    void DirectionalLightPass::PreparePassData(Ref<RenderData> render_data)
    {
        m_render_data = render_data;
    }

    void DirectionalLightPass::DrawForward()
    {
        m_draw_extent   = {m_depth_image.image_extent.width, m_depth_image.image_extent.height};
        auto cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        m_rhi->TransitionImage(cmd_buffer,
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        m_rhi->TransitionImage(cmd_buffer,
                               m_depth_image.image,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        Draw();

        m_rhi->TransitionImage(cmd_buffer,
                               m_depth_image.image,
                               VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                               VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
    }

    void DirectionalLightPass::InitDrawImage()
    {
        VkExtent3D draw_image_extent{
            4096,
            4096,
            1,
        };

        // 设为16位浮点格式以获得更高的精度
        m_draw_image.image_format = VK_FORMAT_R8_SNORM;
        m_draw_image.image_extent = draw_image_extent;

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
    }

    void DirectionalLightPass::InitDepthImage()
    {
        VkExtent3D depth_image_extent{
            4096,
            4096,
            1,
        };

        //----------深度图像----------------
        // 设为32位浮点格式以获得更高的精度
        m_depth_image.image_format = VK_FORMAT_D32_SFLOAT;
        m_depth_image.image_extent = depth_image_extent;

        VkImageUsageFlags depth_image_usages{};
        depth_image_usages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depth_image_usages |= VK_IMAGE_USAGE_SAMPLED_BIT;

        VkImageCreateInfo depth_image_create_info = vkinit::ImageCreateInfo(m_depth_image.image_format, depth_image_usages, m_depth_image.image_extent);

        // 将图像放至GPU only内存
        VmaAllocationCreateInfo depth_image_alloc_info{};
        depth_image_alloc_info.usage         = VMA_MEMORY_USAGE_GPU_ONLY;
        depth_image_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_rhi->CreateDrawImage(&depth_image_create_info, &depth_image_alloc_info, &m_depth_image);

        // 创建图像视图
        VkImageViewCreateInfo depth_view_info = vkinit::ImageViewCreateInfo(m_depth_image.image_format, m_depth_image.image, VK_IMAGE_ASPECT_DEPTH_BIT);

        m_rhi->CreateDrawImageView(&depth_view_info, &m_depth_image);

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

        m_rhi->CreateSampler(&sampler_info, &m_depth_sampler);
    }

    void DirectionalLightPass::InitDescriptors()
    {
        m_descriptor_infos.resize(descriptor_count);

        // 平行光信息
        // {
        //     // 创建描述符布局
        //     DescriptorSetLayoutCreateInfo layout_info{};
        //     layout_info.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        //     layout_info.shader_stages = VK_SHADER_STAGE_VERTEX_BIT;

        //     m_rhi->CreateDescriptorLayout(layout_info, &m_descriptor_infos[light_descriptor].layout);

        //     // 分配描述符集
        //     m_rhi->AllocateDescriptorSets(m_descriptor_infos[light_descriptor].layout, &m_descriptor_infos[light_descriptor].set);

        //     // 写描述符集
        //     DescriptorWriter writer;
        //     writer.WriteBuffer(0, m_global_render_data->directional_light_buffer.buffer, sizeof(DirectionalLightUniformData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        //     m_rhi->UpdateDescriptorSets(writer, m_descriptor_infos[light_descriptor].set);
        // }
    }

    void DirectionalLightPass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);
        {
            //----------shader--------------
            VkShaderModule shadowmap_vert_shader;
            if (!m_rhi->CreateShaderModule(SHADOWMAP_VERT_CODE, &shadowmap_vert_shader))
            {
                LOG_ERROR("Failed to create vertex shader");
            }
            VkShaderModule shadowmap_frag_shader;
            if (!m_rhi->CreateShaderModule(SHADOWMAP_FRAG_CODE, &shadowmap_frag_shader))
            {
                LOG_ERROR("Failed to create fragment shader");
            }

            //------------推送常量------------
            VkPushConstantRange buffer_range{};
            buffer_range.offset     = 0;
            buffer_range.size       = sizeof(PushConstants);
            buffer_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            //-----------管线布局-------------
            std::array<VkDescriptorSetLayout, 0> descriptor_set_layouts{
                // m_descriptor_infos[material_descriptor].layout,
                // m_descriptor_infos[scene_descriptor].layout,
            };

            VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::PipelineLayoutCreateInfo();
            pipeline_layout_info.pushConstantRangeCount     = 1;
            pipeline_layout_info.pPushConstantRanges        = &buffer_range;
            pipeline_layout_info.setLayoutCount             = descriptor_set_layouts.size();
            pipeline_layout_info.pSetLayouts                = descriptor_set_layouts.data();
            m_rhi->CreatePipelineLayout(&pipeline_layout_info, &m_pipelines[main_pipeline].layout);

            //-----------创建管线----------
            DynamicPipelineCreateInfo pipeline_create_info;
            pipeline_create_info.Clear();
            pipeline_create_info.pipeline_layout = m_pipelines[main_pipeline].layout;
            pipeline_create_info.SetShaders(shadowmap_vert_shader, shadowmap_frag_shader);
            pipeline_create_info.SetInputTopolygy(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
            pipeline_create_info.SetPolygonMode(VK_POLYGON_MODE_FILL);
            pipeline_create_info.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            pipeline_create_info.SetMultisamplingNone();
            pipeline_create_info.DisableBlending();
            pipeline_create_info.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
            pipeline_create_info.SetDepthFormat(m_depth_image.image_format);

            m_rhi->CreateDynamicPipelines(pipeline_create_info, &m_pipelines[main_pipeline].pipeline);

            //--------清除---------
            m_rhi->DestroyShaderModule(shadowmap_vert_shader);
            m_rhi->DestroyShaderModule(shadowmap_frag_shader);
        }
    }

    void DirectionalLightPass::Draw()
    {
        VkCommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        //---------渲染信息----------------
        // 清除信息
        VkClearValue clear_value{};
        clear_value.color        = {{0.0f}};
        clear_value.depthStencil = {0.0f, 0};

        // 颜色附件
        VkRenderingAttachmentInfo color_attachment = vkinit::AttachmentInfo(m_draw_image.image_view, &clear_value, VK_IMAGE_LAYOUT_GENERAL);
        // 深度附件
        VkRenderingAttachmentInfo depth_attachment = vkinit::DepthAttachmentInfo(m_depth_image.image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

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

        for (auto& pair1 : m_render_data->objects)
        {
            for (auto& pair2 : pair1.second)
            {
                Ref<VulkanMesh> mesh = pair2.first;

                // 推送常量
                // 将MVP矩阵和顶点的设备地址传入
                PushConstants push_constants;
                push_constants.light_MVP     = m_render_data->directional_light_MVP;
                push_constants.vertex_buffer = mesh->vertex_buffer_address;
                vkCmdPushConstants(cmd_buffer, m_pipelines[pipelines::main_pipeline].layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &push_constants);

                // 绑定IBO
                vkCmdBindIndexBuffer(cmd_buffer, mesh->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

                vkCmdDrawIndexed(cmd_buffer, static_cast<uint32_t>(mesh->index_count), 1, 0, 0, 0);
            }
        }
        vkCmdEndRendering(cmd_buffer);
    }

} // namespace Yutrel