#include "yutrel_pch.hpp"

#include "base_pass.hpp"

#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_asset.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include <vector>
#include <vulkan/vulkan_structs.hpp>

namespace Yutrel
{
    void BasePass::Init(RenderPassInitInfo* info)
    {
        RenderPass::Init(info);

        InitDrawImage();

        InitDepthImage();

        InitUnifromBuffers();

        InitDescriptors();

        InitPipelines();
    }

    void BasePass::Draw()
    {
        m_draw_extent = m_rhi->GetSwapChainExtent();

        UpdateUniformBuffer();

        //--------绘制------------
        vk::CommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        // 图像格式转换为颜色缓冲
        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_base_color.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eColorAttachmentOptimal);
        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_world_normal.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eColorAttachmentOptimal);
        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_world_position.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eColorAttachmentOptimal);
        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_metallic_roughness.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eColorAttachmentOptimal);
        // 深度图像格式转换为深度附件
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_depth_image.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eDepthAttachmentOptimal);

        // 绘制
        DrawGeometry();

        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_base_color.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eShaderReadOnlyOptimal);
        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_world_normal.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eShaderReadOnlyOptimal);
        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_world_position.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eShaderReadOnlyOptimal);
        m_rhi->TransitionImage(cmd_buffer,
                               gbuffer_metallic_roughness.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    void BasePass::InitDrawImage()
    {
        vk::Extent3D draw_image_extent{
            3840,
            2160,
            1,
        };

        vk::ImageUsageFlags draw_image_usages{};
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferSrc;
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferDst;
        draw_image_usages |= vk::ImageUsageFlagBits::eStorage;
        draw_image_usages |= vk::ImageUsageFlagBits::eColorAttachment;
        draw_image_usages |= vk::ImageUsageFlagBits::eSampled;

        // base color
        gbuffer_base_color.format = vk::Format::eR16G16B16A16Sfloat;
        gbuffer_base_color.extent = draw_image_extent;
        gbuffer_base_color        = m_rhi->CreateImage(draw_image_extent, gbuffer_base_color.format, draw_image_usages);

        // normal
        gbuffer_world_normal.format = vk::Format::eR16G16B16A16Sfloat;
        gbuffer_world_normal.extent = draw_image_extent;
        gbuffer_world_normal        = m_rhi->CreateImage(draw_image_extent, gbuffer_world_normal.format, draw_image_usages);

        // position
        gbuffer_world_position.format = vk::Format::eR16G16B16A16Sfloat;
        gbuffer_world_position.extent = draw_image_extent;
        gbuffer_world_position        = m_rhi->CreateImage(draw_image_extent, gbuffer_world_position.format, draw_image_usages);

        // metallic_roughness
        gbuffer_metallic_roughness.format = vk::Format::eR16G16B16A16Sfloat;
        gbuffer_metallic_roughness.extent = draw_image_extent;
        gbuffer_metallic_roughness        = m_rhi->CreateImage(draw_image_extent, gbuffer_metallic_roughness.format, draw_image_usages);
    }

    void BasePass::InitDepthImage()
    {
        m_depth_image.format = vk::Format::eD32Sfloat;
        m_depth_image.extent = gbuffer_base_color.extent;

        m_depth_image = m_rhi->CreateImage(m_depth_image.extent, m_depth_image.format, vk::ImageUsageFlagBits::eDepthStencilAttachment);
    }

    void BasePass::InitUnifromBuffers()
    {
        m_scene_uniform_buffer = m_rhi->CreateBuffer(sizeof(m_scene_uniform_data), vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu);
    }

    void BasePass::InitDescriptors()
    {
        m_descriptors.resize(descriptor_count);

        // 场景信息
        {
            // 创建描述符布局
            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eUniformBuffer)
                    .SetShaderStage(vk::ShaderStageFlagBits::eAllGraphics);

            m_descriptors[scene_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            // 分配描述符集
            m_descriptors[scene_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[scene_descriptor].layout);

            auto writer =
                DescriptorWriter()
                    .WriteBuffer(0, m_scene_uniform_buffer.buffer, sizeof(m_scene_uniform_data), 0, vk::DescriptorType::eUniformBuffer);
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

    void BasePass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);

        //-----------main-----------
        {
            //----------shader--------------
            vk::ShaderModule base_pass_vert_shader = m_rhi->CreateShaderModule(BASE_PASS_VERT_CODE);
            vk::ShaderModule base_pass_frag_shader = m_rhi->CreateShaderModule(BASE_PASS_FRAG_CODE);

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
                    .SetShaders(base_pass_vert_shader, base_pass_frag_shader)
                    .SetInputTopolygy(vk::PrimitiveTopology::eTriangleList)
                    .SetPolygonMode(vk::PolygonMode::eFill)
                    .SetCullMode(vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise)
                    .SetMultisamplingNone()
                    .DisableBlending()
                    .EnableDepthTest(vk::True, vk::CompareOp::eGreaterOrEqual)
                    .SetColorAttachmentFormats({gbuffer_base_color.format,
                                                gbuffer_world_normal.format,
                                                gbuffer_world_position.format,
                                                gbuffer_metallic_roughness.format})
                    .SetDepthFormat(m_depth_image.format);

            m_pipelines[main_pipeline].pipeline = m_rhi->CreateRenderPipeline(render_pipeline_ci);

            //--------清除---------
            m_rhi->DestroyShaderModule(base_pass_vert_shader);
            m_rhi->DestroyShaderModule(base_pass_frag_shader);
        }
    }

    void BasePass::UpdateUniformBuffer()
    {
        m_scene_uniform_data.view       = m_render_scene->view_matrix;
        m_scene_uniform_data.projection = m_render_scene->projection_matrix;

        memcpy(m_scene_uniform_buffer.info.pMappedData, &m_scene_uniform_data, sizeof(m_scene_uniform_data));
    }

    void BasePass::DrawGeometry()
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
                .setColor(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));

        auto depth_clear_value =
            vk::ClearValue()
                .setDepthStencil(vk::ClearDepthStencilValue(0.0f));

        // 颜色附件
        std::vector<vk::RenderingAttachmentInfo> color_attachments;

        auto color_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(gbuffer_base_color.image_view)
                .setImageLayout(vk::ImageLayout::eGeneral)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setClearValue(color_clear_value);
        color_attachments.push_back(color_attachment);

        color_attachment.setImageView(gbuffer_world_normal.image_view);
        color_attachments.push_back(color_attachment);

        color_attachment.setImageView(gbuffer_world_position.image_view);
        color_attachments.push_back(color_attachment);

        color_attachment.setImageView(gbuffer_metallic_roughness.image_view);
        color_attachments.push_back(color_attachment);

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
                .setColorAttachments(color_attachments)
                .setPDepthAttachment(&depth_attachment);

        cmd_buffer.beginRendering(render_info);

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

        // 绑定管线
        cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].pipeline);

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