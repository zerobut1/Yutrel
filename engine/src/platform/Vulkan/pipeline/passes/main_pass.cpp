#include "yutrel_pch.hpp"

#include "main_pass.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_asset.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"

#include <array>
#include <stdint.h>
#include <vcruntime.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

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

        InitUnifromBuffers();

        InitDescriptors();

        InitPipelines();
    }

    void MainPass::DrawForward()
    {
        m_draw_extent = m_rhi->GetSwapChainExtent();

        UpdateUniformBuffer();

        //--------绘制------------
        vk::CommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        // 图像格式转换为颜色缓冲
        m_rhi->TransitionImage(cmd_buffer,
                               m_draw_image.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eColorAttachmentOptimal);
        // 深度图像格式转换为深度附件
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_depth_image.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eDepthAttachmentOptimal);

        // 绘制
        DrawGeometry();

        // 将渲染图像布局转换为传输源布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_draw_image.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eTransferSrcOptimal);

        //-----------------------
        CopyToSwapchain();
    }

    void MainPass::InitDrawImage()
    {
        vk::Extent3D draw_image_extent{
            3840,
            2160,
            1,
        };

        // 设为16位浮点格式以获得更高的精度
        m_draw_image.format = vk::Format::eR16G16B16A16Sfloat;
        m_draw_image.extent = draw_image_extent;

        vk::ImageUsageFlags draw_image_usages{};
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferSrc;
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferDst;
        draw_image_usages |= vk::ImageUsageFlagBits::eStorage;
        draw_image_usages |= vk::ImageUsageFlagBits::eColorAttachment;

        m_draw_image = m_rhi->CreateImage(draw_image_extent, m_draw_image.format, draw_image_usages);
    }

    void MainPass::InitDepthImage()
    {
        m_depth_image.format = vk::Format::eD32Sfloat;
        m_depth_image.extent = m_draw_image.extent;

        m_depth_image = m_rhi->CreateImage(m_depth_image.extent, m_depth_image.format, vk::ImageUsageFlagBits::eDepthStencilAttachment);
    }

    void MainPass::InitUnifromBuffers()
    {
        m_scene_uniform_buffer = m_rhi->CreateBuffer(sizeof(m_scene_uniform_data), vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu);
    }

    void MainPass::InitDescriptors()
    {
        m_descriptors.resize(descriptor_count);

        // 场景信息
        {
            // 创建描述符布局
            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eUniformBuffer)
                    .AddBinding(1, vk::DescriptorType::eCombinedImageSampler)
                    .SetShaderStage(vk::ShaderStageFlagBits::eAllGraphics);

            m_descriptors[scene_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            // 分配描述符集
            m_descriptors[scene_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[scene_descriptor].layout);

            // 写描述符集
            auto writer =
                DescriptorWriter()
                    .WriteBuffer(0, m_scene_uniform_buffer.buffer, sizeof(m_scene_uniform_data), 0, vk::DescriptorType::eUniformBuffer)
                    .WriteImage(1, m_directional_light_shadowmap.image_view, m_shadowmap_sampler, vk::ImageLayout::eDepthStencilReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
            m_rhi->UpdateDescriptorSets(writer, m_descriptors[scene_descriptor].set);
        }

        // 材质
        {
            DescriptorSetLayoutCreateInfo layout_info;

            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eUniformBuffer)
                    .AddBinding(1, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(2, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(3, vk::DescriptorType::eCombinedImageSampler)
                    .SetShaderStage(vk::ShaderStageFlagBits::eFragment);

            m_descriptors[material_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);
        }
    }

    void MainPass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);

        {
            //----------shader--------------
            vk::ShaderModule triangle_vert_shader = m_rhi->CreateShaderModule(TRIANGLE_VERT_CODE);
            vk::ShaderModule texture_frag_shader  = m_rhi->CreateShaderModule(TEXTURE_FRAG_CODE);

            //------------推送常量------------
            auto push_constant_range =
                vk::PushConstantRange()
                    .setOffset(0)
                    .setSize(sizeof(m_push_constants))
                    .setStageFlags(vk::ShaderStageFlagBits::eVertex);

            //-----------管线布局-------------
            std::vector<vk::DescriptorSetLayout> descriptor_set_layouts{
                m_descriptors[scene_descriptor].layout,
                m_descriptors[material_descriptor].layout,
            };

            auto pipeline_ci =
                vk::PipelineLayoutCreateInfo()
                    .setPushConstantRanges(push_constant_range)
                    .setSetLayouts(descriptor_set_layouts);
            m_pipelines[main_pipeline].layout = m_rhi->CreatePipelineLayout(pipeline_ci);

            //-----------创建管线----------
            auto render_pipeline_ci =
                RenderPipelineCreateInfo()
                    .SetPipelineLayout(m_pipelines[main_pipeline].layout)
                    .SetShaders(triangle_vert_shader, texture_frag_shader)
                    .SetInputTopolygy(vk::PrimitiveTopology::eTriangleList)
                    .SetPolygonMode(vk::PolygonMode::eFill)
                    .SetCullMode(vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise)
                    .SetMultisamplingNone()
                    .DisableBlending()
                    .EnableDepthTest(vk::True, vk::CompareOp::eGreaterOrEqual)
                    .SetColorAttachmentFormat(m_draw_image.format)
                    .SetDepthFormat(m_depth_image.format);

            m_pipelines[main_pipeline].pipeline = m_rhi->CreateRenderPipeline(render_pipeline_ci);

            //--------清除---------
            m_rhi->DestroyShaderModule(triangle_vert_shader);
            m_rhi->DestroyShaderModule(texture_frag_shader);
        }
    }

    void MainPass::CopyToSwapchain()
    {
        // 将交换链布局转换为传输目标布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_rhi->GetCurrentSwapchainImage(),
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eTransferDstOptimal);

        // 将渲染图像拷贝至交换链
        m_rhi->CopyImageToImage(m_rhi->GetCurrentCommandBuffer(),
                                m_draw_image.image,
                                m_rhi->GetCurrentSwapchainImage(),
                                m_draw_extent,
                                m_rhi->GetSwapChainExtent());
    }

    void MainPass::UpdateUniformBuffer()
    {
        m_scene_uniform_data.view                        = m_render_scene->view_matrix;
        m_scene_uniform_data.projection                  = m_render_scene->projection_matrix;
        m_scene_uniform_data.camera_position             = m_render_scene->camera_position;
        m_scene_uniform_data.ambient_color               = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);
        m_scene_uniform_data.directional_light_color     = glm::vec4(m_render_scene->directional_light.color, m_render_scene->directional_light.intensity);
        m_scene_uniform_data.directional_light_direction = glm::vec4(m_render_scene->directional_light.direction, 1.0f);

        for (size_t i = 0; i < 4; i++)
        {
            m_scene_uniform_data.directional_light_VP[i] = m_render_scene->directional_light_VP[i];
            m_scene_uniform_data.cascade_splits[i]       = m_render_scene->cascade_splits[i] * -1.0f;
        }

        memcpy(m_scene_uniform_buffer.info.pMappedData, &m_scene_uniform_data, sizeof(m_scene_uniform_data));
    }

    void MainPass::DrawGeometry()
    {
        // 渲染数据
        std::unordered_map<Ref<VulkanPBRMaterial>, std::unordered_map<Ref<VulkanMesh>, std::vector<glm::mat4>>> main_camera_mesh_drawcall_batch;

        for (auto& object : m_render_scene->render_entities)
        {
            auto& mesh_instanced = main_camera_mesh_drawcall_batch[object.material];
            auto& mesh_nodes     = mesh_instanced[object.mesh];

            mesh_nodes.push_back(object.model_matrix);
        }

        vk::CommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        //---------渲染信息----------------
        // 清除信息
        auto color_clear_value =
            vk::ClearValue()
                .setColor(vk::ClearColorValue(0.4f, 0.8f, 1.0f, 1.0f));

        auto depth_clear_value =
            vk::ClearValue()
                .setDepthStencil(vk::ClearDepthStencilValue(0.0f));

        // 颜色附件
        auto color_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(m_draw_image.image_view)
                .setImageLayout(vk::ImageLayout::eGeneral)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setClearValue(color_clear_value);

        // 深度附件
        auto depth_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(m_depth_image.image_view)
                .setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setClearValue(depth_clear_value);

        // 开始渲染
        auto render_info =
            vk::RenderingInfo()
                .setRenderArea(vk::Rect2D({0, 0}, m_draw_extent))
                .setLayerCount(1)
                .setColorAttachments(color_attachment)
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

        for (auto& pair1 : main_camera_mesh_drawcall_batch)
        {
            Ref<VulkanPBRMaterial> material = pair1.first;
            auto& mesh_instance             = pair1.second;

            // 绑定材质描述符
            cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].layout, 1, material->descriptor_set, {});

            for (auto& pair2 : mesh_instance)
            {
                Ref<VulkanMesh> mesh = pair2.first;
                auto& transform      = pair2.second;

                // 推送常量
                // 将模型矩阵和顶点的设备地址传入
                m_push_constants.model_matrix  = transform[0];
                m_push_constants.vertex_buffer = mesh->vertex_buffer_address;

                cmd_buffer.pushConstants(m_pipelines[main_pipeline].layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(m_push_constants), &m_push_constants);

                // 绑定IBO
                cmd_buffer.bindIndexBuffer(mesh->index_buffer.buffer, 0, vk::IndexType::eUint32);

                cmd_buffer.drawIndexed(mesh->index_count, 1, 0, 0, 0);
            }
        }
        cmd_buffer.endRendering();
    }

} // namespace Yutrel