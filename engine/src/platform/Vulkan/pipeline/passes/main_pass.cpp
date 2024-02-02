#include "yutrel_pch.hpp"

#include "main_pass.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_asset.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"

#include <array>
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    void MainPass::Init(RenderPassInitInfo* info)
    {
        RenderPass::Init(info);

        auto _info = static_cast<MainPassInitInfo*>(info);

        m_directional_light_shadowmap = _info->directional_light_shadowmap;
        m_shadowmap_sampler           = _info->shadowmap_sampler;

        InitDrawImage();

        InitDepthImage();

        InitDescriptors();

        InitPipelines();
    }

    void MainPass::DrawForward()
    {
        PrepareDrawImage();

        //--------绘制------------
        // 图像格式转换为颜色缓冲
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        // 深度图像格式转换为深度附件
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_depth_image.image,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        // 绘制几何图形
        DrawGeometry();

        // 将渲染图像布局转换为传输源布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                               VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        //-----------------------
        CopyToSwapchain();
    }

    void MainPass::InitDrawImage()
    {
        VkExtent3D draw_image_extent{
            3840,
            2160,
            1,
        };

        // 设为16位浮点格式以获得更高的精度
        m_draw_image.image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
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

    void MainPass::InitDepthImage()
    {
        m_depth_image.image_format = VK_FORMAT_D32_SFLOAT;
        m_depth_image.image_extent = m_draw_image.image_extent;

        VkImageUsageFlags depth_image_usages{};
        depth_image_usages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkImageCreateInfo depth_image_info = vkinit::ImageCreateInfo(m_depth_image.image_format, depth_image_usages, m_depth_image.image_extent);

        VmaAllocationCreateInfo depth_image_alloc_info{};
        depth_image_alloc_info.usage         = VMA_MEMORY_USAGE_GPU_ONLY;
        depth_image_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_rhi->CreateDrawImage(&depth_image_info, &depth_image_alloc_info, &m_depth_image);

        VkImageViewCreateInfo depth_view_info = vkinit::ImageViewCreateInfo(m_depth_image.image_format, m_depth_image.image, VK_IMAGE_ASPECT_DEPTH_BIT);

        m_rhi->CreateDrawImageView(&depth_view_info, &m_depth_image);
    }

    void MainPass::InitDescriptors()
    {
        m_descriptor_infos.resize(descriptor_count);

        // 场景信息
        {
            // 创建描述符布局
            DescriptorSetLayoutCreateInfo layout_info{};
            layout_info.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            layout_info.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            layout_info.shader_stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

            m_rhi->CreateDescriptorLayout(layout_info, &m_descriptor_infos[scene_descriptor].layout);

            // 分配描述符集
            m_rhi->AllocateDescriptorSets(m_descriptor_infos[scene_descriptor].layout, &m_descriptor_infos[scene_descriptor].set);

            // 写描述符集
            DescriptorWriter writer;
            writer.WriteBuffer(0, m_asset_manager->GetGlobalRenderData()->scene_buffer.buffer, sizeof(SceneUniformData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            writer.WriteImage(1, m_directional_light_shadowmap.image_view, m_shadowmap_sampler, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            m_rhi->UpdateDescriptorSets(writer, m_descriptor_infos[scene_descriptor].set);
        }

        // 材质
        {
            DescriptorSetLayoutCreateInfo layout_info;
            layout_info.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            layout_info.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            layout_info.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            layout_info.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            layout_info.shader_stages = VK_SHADER_STAGE_FRAGMENT_BIT;

            m_rhi->CreateDescriptorLayout(layout_info, &m_descriptor_infos[material_descriptor].layout);
        }
    }

    void MainPass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);

        {
            //----------shader--------------
            VkShaderModule triangle_vert_shader;
            if (!m_rhi->CreateShaderModule(TRIANGLE_VERT_CODE, &triangle_vert_shader))
            {
                LOG_ERROR("Failed to create vertex shader");
            }
            VkShaderModule texture_frag_shader;
            if (!m_rhi->CreateShaderModule(TEXTURE_FRAG_CODE, &texture_frag_shader))
            {
                LOG_ERROR("Failed to create fragment shader");
            }

            //------------推送常量------------
            VkPushConstantRange buffer_range{};
            buffer_range.offset     = 0;
            buffer_range.size       = sizeof(m_push_constants);
            buffer_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            //-----------管线布局-------------
            std::array<VkDescriptorSetLayout, 2> descriptor_set_layouts{
                m_descriptor_infos[material_descriptor].layout,
                m_descriptor_infos[scene_descriptor].layout,
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
            pipeline_create_info.SetShaders(triangle_vert_shader, texture_frag_shader);
            pipeline_create_info.SetInputTopolygy(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
            pipeline_create_info.SetPolygonMode(VK_POLYGON_MODE_FILL);
            pipeline_create_info.SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
            pipeline_create_info.SetMultisamplingNone();
            pipeline_create_info.DisableBlending();
            // pipeline_create_info.EnableBlendingAdditive();
            // pipeline_create_info.EnableBlendingAlphablend();
            pipeline_create_info.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
            pipeline_create_info.SetColorAttachmentFormat(m_draw_image.image_format);
            pipeline_create_info.SetDepthFormat(m_depth_image.image_format);

            m_rhi->CreateDynamicPipelines(pipeline_create_info, &m_pipelines[main_pipeline].pipeline);

            //--------清除---------
            m_rhi->DestroyShaderModule(triangle_vert_shader);
            m_rhi->DestroyShaderModule(texture_frag_shader);
        }
    }

    void MainPass::PrepareDrawImage()
    {
        // 设置渲染图像范围
        m_draw_extent = m_rhi->GetSwapChainInfo().extent;
    }

    void MainPass::CopyToSwapchain()
    {
        // 将交换链布局转换为传输目标布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_rhi->GetCurrentSwapchainImage(),
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // 将渲染图像拷贝至交换链
        m_rhi->CopyImageToImage(m_rhi->GetCurrentCommandBuffer(),
                                m_draw_image.image,
                                m_rhi->GetCurrentSwapchainImage(),
                                m_draw_extent,
                                m_rhi->GetSwapChainInfo().extent);
    }

    void MainPass::DrawGeometry()
    {

        VkCommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        //---------渲染信息----------------
        // 清除信息
        VkClearValue clear_value{};
        clear_value.color = {{0.4f, 0.8f, 1.0f, 1.0f}};

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

        // 绑定全局变量描述符
        vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[main_pipeline].layout, 1, 1, &m_descriptor_infos[scene_descriptor].set, 0, nullptr);

        for (auto& pair1 : m_render_data->objects)
        {
            Ref<VulkanPBRMaterial> material = pair1.first;
            auto& mesh_instance             = pair1.second;

            // 绑定材质描述符
            vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[main_pipeline].layout, 0, 1, &material->descriptor_set, 0, nullptr);

            for (auto& pair2 : mesh_instance)
            {
                Ref<VulkanMesh> mesh = pair2.first;
                auto& transform      = pair2.second;

                // 推送常量
                // 将模型矩阵和顶点的设备地址传入
                m_push_constants.model_matrix         = transform[0];
                m_push_constants.directional_light_VP = m_render_data->directional_light_VP;
                m_push_constants.vertex_buffer        = mesh->vertex_buffer_address;
                vkCmdPushConstants(cmd_buffer, m_pipelines[pipelines::main_pipeline].layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(m_push_constants), &m_push_constants);

                // 绑定IBO
                vkCmdBindIndexBuffer(cmd_buffer, mesh->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

                vkCmdDrawIndexed(cmd_buffer, static_cast<uint32_t>(mesh->index_count), 1, 0, 0, 0);
            }
        }
        vkCmdEndRendering(cmd_buffer);
    }

} // namespace Yutrel