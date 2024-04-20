#include "yutrel_pch.hpp"

#include "water_pass.hpp"

#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_asset.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include <vulkan/vulkan_enums.hpp>

namespace Yutrel
{
    void WaterPass::Init(RenderPassInitInfo* info)
    {
        RenderPass::Init(info);

        auto _info = static_cast<WaterPassInitInfo*>(info);

        InitImages();

        m_draw_image = _info->draw_image;

        m_depth_image = _info->depth_image;

        InitUnifromBuffers();

        InitDescriptors();

        InitPipelines();
    }

    void WaterPass::Draw()
    {
        static bool is_water_loaded = false;

        if (!is_water_loaded)
        {
            auto writer =
                DescriptorWriter()
                    .WriteBuffer(0, m_scene_uniform_buffer.buffer, sizeof(m_scene_uniform_data), 0, vk::DescriptorType::eUniformBuffer)
                    .WriteImage(1, m_render_scene->water_normal.image_view, m_asset_manager->m_default_data.linear_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(2, m_render_scene->skybox->brdf_lut.image_view, m_asset_manager->m_default_data.linear_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(3, m_render_scene->skybox->prefiltered.image_view, m_asset_manager->m_default_data.linear_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(4, m_render_scene->skybox->irradiance.image_view, m_asset_manager->m_default_data.linear_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(5, m_scene_color.image_view, m_asset_manager->m_default_data.linear_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(6, m_scene_depth.image_view, m_asset_manager->m_default_data.nearset_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
            m_rhi->UpdateDescriptorSets(writer, m_descriptors[scene_descriptor].set);

            is_water_loaded = true;
        }

        m_draw_extent = m_rhi->GetSwapChainExtent();

        UpdateUniformBuffer();

        //--------绘制------------
        vk::CommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        // 拷贝场景颜色
        m_rhi->TransitionImage(cmd_buffer,
                               m_draw_image.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eTransferSrcOptimal);

        m_rhi->TransitionImage(cmd_buffer,
                               m_scene_color.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eTransferDstOptimal);

        m_rhi->CopyImageToImage(cmd_buffer,
                                m_draw_image.image,
                                m_scene_color.image,
                                m_draw_extent,
                                m_draw_extent);

        m_rhi->TransitionImage(cmd_buffer,
                               m_draw_image.image,
                               vk::ImageLayout::eTransferSrcOptimal,
                               vk::ImageLayout::eColorAttachmentOptimal);

        m_rhi->TransitionImage(cmd_buffer,
                               m_scene_color.image,
                               vk::ImageLayout::eTransferDstOptimal,
                               vk::ImageLayout::eShaderReadOnlyOptimal);

        // 拷贝深度图
        m_rhi->TransitionDepthImage(cmd_buffer,
                                    m_depth_image.image,
                                    vk::ImageLayout::eDepthAttachmentOptimal,
                                    vk::ImageLayout::eTransferSrcOptimal);

        m_rhi->TransitionDepthImage(cmd_buffer,
                                    m_scene_depth.image,
                                    vk::ImageLayout::eUndefined,
                                    vk::ImageLayout::eTransferDstOptimal);

        m_rhi->CopyDepthImageToImage(cmd_buffer,
                                     m_depth_image.image,
                                     m_scene_depth.image,
                                     m_draw_extent,
                                     m_draw_extent);

        m_rhi->TransitionDepthImage(cmd_buffer,
                                    m_depth_image.image,
                                    vk::ImageLayout::eTransferSrcOptimal,
                                    vk::ImageLayout::eDepthAttachmentOptimal);

        m_rhi->TransitionDepthImage(cmd_buffer,
                                    m_scene_depth.image,
                                    vk::ImageLayout::eTransferDstOptimal,
                                    vk::ImageLayout::eShaderReadOnlyOptimal);

        // 绘制
        DrawGeometry();

        // 将渲染图像布局转换为传输源布局
        m_rhi->TransitionImage(cmd_buffer,
                               m_draw_image.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eTransferSrcOptimal);

        //-----------------------
        CopyToSwapchain();
    }

    void WaterPass::InitImages()
    {
        vk::Extent3D image_extent{
            3840,
            2160,
            1,
        };

        vk::ImageUsageFlags scene_color_usages{};
        scene_color_usages |= vk::ImageUsageFlagBits::eTransferSrc;
        scene_color_usages |= vk::ImageUsageFlagBits::eTransferDst;
        scene_color_usages |= vk::ImageUsageFlagBits::eStorage;
        scene_color_usages |= vk::ImageUsageFlagBits::eColorAttachment;
        scene_color_usages |= vk::ImageUsageFlagBits::eSampled;

        // scene color
        m_scene_color.format = vk::Format::eR16G16B16A16Sfloat;
        m_scene_color.extent = image_extent;

        m_scene_color = m_rhi->CreateImage(image_extent, m_scene_color.format, scene_color_usages);

        // scene depth
        m_scene_depth.format = vk::Format::eD32Sfloat;
        m_scene_depth.extent = image_extent;

        m_scene_depth = m_rhi->CreateImage(image_extent, m_scene_depth.format, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
    }

    void WaterPass::InitUnifromBuffers()
    {
        m_scene_uniform_buffer = m_rhi->CreateBuffer(sizeof(m_scene_uniform_data), vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu);
    }

    void WaterPass::InitDescriptors()
    {
        m_descriptors.resize(descriptor_count);

        // 场景信息
        {
            // 创建描述符布局
            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eUniformBuffer)
                    .AddBinding(1, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(2, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(3, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(4, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(5, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(6, vk::DescriptorType::eCombinedImageSampler)
                    .SetShaderStage(vk::ShaderStageFlagBits::eAllGraphics);

            m_descriptors[scene_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            // 分配描述符集
            m_descriptors[scene_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[scene_descriptor].layout);
        }

        // scenecolor
        {
            // 创建描述符布局
            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eCombinedImageSampler)
                    .SetShaderStage(vk::ShaderStageFlagBits::eAllGraphics);

            m_descriptors[color_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            // 分配描述符集
            m_descriptors[color_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[color_descriptor].layout);
        }
    }

    void WaterPass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);

        //-----------main-----------
        {
            //----------shader--------------
            vk::ShaderModule water_vert_shader = m_rhi->CreateShaderModule(WATER_VERT_CODE);
            vk::ShaderModule water_frag_shader = m_rhi->CreateShaderModule(WATER_FRAG_CODE);

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
                    .setPushConstantRanges(push_constant_range)
                    .setSetLayouts(descriptor_set_layouts);
            m_pipelines[main_pipeline].layout = m_rhi->CreatePipelineLayout(pipeline_ci);

            //-----------创建管线----------
            auto render_pipeline_ci =
                RenderPipelineCreateInfo()
                    .SetPipelineLayout(m_pipelines[main_pipeline].layout)
                    .SetShaders(water_vert_shader, water_frag_shader)
                    .SetInputTopolygy(vk::PrimitiveTopology::eTriangleList)
                    .SetPolygonMode(vk::PolygonMode::eFill)
                    .SetCullMode(vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise)
                    .SetMultisamplingNone()
                    .EnableBlendingAlphablend()
                    // .DisableBlending()
                    .EnableDepthTest(vk::True, vk::CompareOp::eGreaterOrEqual)
                    .SetColorAttachmentFormat(m_draw_image.format)
                    .SetDepthFormat(m_depth_image.format);

            m_pipelines[main_pipeline].pipeline = m_rhi->CreateRenderPipeline(render_pipeline_ci);

            //--------清除---------
            m_rhi->DestroyShaderModule(water_vert_shader);
            m_rhi->DestroyShaderModule(water_frag_shader);
        }
    }

    void WaterPass::CopyToSwapchain()
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

    void WaterPass::UpdateUniformBuffer()
    {
        m_scene_uniform_data.view                        = m_render_scene->view_matrix;
        m_scene_uniform_data.projection                  = m_render_scene->projection_matrix;
        m_scene_uniform_data.camera_position             = m_render_scene->camera_position;
        m_scene_uniform_data.directional_light_color     = glm::vec4(m_render_scene->directional_light.color, m_render_scene->directional_light.intensity);
        m_scene_uniform_data.directional_light_direction = glm::vec4(m_render_scene->directional_light.direction, 1.0f);
        m_scene_uniform_data.near_plane                  = m_render_scene->near_plane;
        m_scene_uniform_data.far_plane                   = m_render_scene->far_plane;

        memcpy(m_scene_uniform_buffer.info.pMappedData, &m_scene_uniform_data, sizeof(m_scene_uniform_data));
    }

    void WaterPass::DrawGeometry()
    {
        vk::CommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        //---------渲染信息----------------
        // 颜色附件
        auto color_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(m_draw_image.image_view)
                .setImageLayout(vk::ImageLayout::eGeneral)
                .setLoadOp(vk::AttachmentLoadOp::eLoad)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setClearValue({});

        // 深度附件
        auto depth_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(m_depth_image.image_view)
                .setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal)
                .setLoadOp(vk::AttachmentLoadOp::eLoad)
                .setStoreOp(vk::AttachmentStoreOp::eNone)
                .setClearValue({});

        // 开始渲染
        auto render_info =
            vk::RenderingInfo()
                .setRenderArea(vk::Rect2D({0, 0}, m_draw_extent))
                .setLayerCount(1)
                .setColorAttachments(color_attachment)
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
        // cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].layout, 1, m_descriptors[color_descriptor].set, {});

        // 绑定管线
        cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].pipeline);

        m_push_constants.model_matrix  = m_render_scene->water_transform.model_matrix;
        m_push_constants.vertex_buffer = m_render_scene->water_plane->vertex_buffer_address;
        m_push_constants.width         = m_rhi->GetSwapChainExtent().width;
        m_push_constants.height        = m_rhi->GetSwapChainExtent().height;

        static float time     = 0.0;
        m_push_constants.time = time;
        time += 1.0;

        cmd_buffer.pushConstants(m_pipelines[main_pipeline].layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(m_push_constants), &m_push_constants);

        // 绑定IBO
        cmd_buffer.bindIndexBuffer(m_render_scene->water_plane->index_buffer.buffer, 0, vk::IndexType::eUint32);

        cmd_buffer.drawIndexed(m_render_scene->water_plane->index_count, 1, 0, 0, 0);

        cmd_buffer.endRendering();
    }

} // namespace Yutrel