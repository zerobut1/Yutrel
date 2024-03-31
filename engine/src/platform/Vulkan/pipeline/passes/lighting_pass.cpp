#include "yutrel_pch.hpp"

#include "lighting_pass.hpp"

#include "platform/Vulkan/asset/shaders.hpp"
#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include <vulkan/vulkan_enums.hpp>

namespace Yutrel
{
    void LightingPass::Init(RenderPassInitInfo* info)
    {
        RenderPass::Init(info);

        auto _info = static_cast<LightingPassInitInfo*>(info);

        gbuffer_base_color         = _info->gbuffer_base_color;
        gbuffer_world_normal       = _info->gbuffer_world_normal;
        gbuffer_world_position     = _info->gbuffer_world_position;
        gbuffer_metallic_roughness = _info->gbuffer_metallic_roughness;
        depth_image                = _info->gbuffer_depth;

        m_directional_light_shadowmap = _info->directional_light_shadowmap;
        m_shadowmap_sampler           = _info->shadowmap_sampler;

        InitGbuffer(_info);

        InitDrawImage();

        InitUnifromBuffers();

        InitDescriptors();

        InitPipelines();
    }

    void LightingPass::Draw()
    {
        m_draw_extent = m_rhi->GetSwapChainExtent();

        UpdateUniformBuffer();

        static bool is_skybox_loaded = false;

        if (!is_skybox_loaded)
        {
            // 场景
            auto writer =
                DescriptorWriter()
                    .WriteBuffer(0, m_scene_uniform_buffer.buffer, sizeof(m_scene_uniform_data), 0, vk::DescriptorType::eUniformBuffer)
                    .WriteImage(1, m_directional_light_shadowmap.image_view, m_shadowmap_sampler, vk::ImageLayout::eDepthStencilReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(2, m_render_scene->skybox->brdf_lut.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(3, m_render_scene->skybox->prefiltered.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(4, m_render_scene->skybox->irradiance.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
            m_rhi->UpdateDescriptorSets(writer, m_descriptors[scene_descriptor].set);

            // skybox
            writer =
                DescriptorWriter()
                    .WriteBuffer(0, m_scene_uniform_buffer.buffer, sizeof(m_scene_uniform_data), 0, vk::DescriptorType::eUniformBuffer)
                    .WriteImage(1, m_render_scene->skybox->prefiltered.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
            m_rhi->UpdateDescriptorSets(writer, m_descriptors[skybox_descriptor].set);
            is_skybox_loaded = true;
        }

        //--------绘制------------
        vk::CommandBuffer cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        // 图像格式转换为颜色缓冲
        m_rhi->TransitionImage(cmd_buffer,
                               draw_image.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eColorAttachmentOptimal);
        // 深度图像格式转换为深度附件
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               depth_image.image,
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eDepthAttachmentOptimal);

        // 绘制
        DrawGeometry();

        // 将渲染图像布局转换为传输源布局
        m_rhi->TransitionImage(cmd_buffer,
                               draw_image.image,
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::eTransferSrcOptimal);

        CopyToSwapchain();
    }

    void LightingPass::InitGbuffer(LightingPassInitInfo* info)
    {
        vk::Extent3D gbuffer_extent{
            m_rhi->GetSwapChainExtent(),
            1,
        };

        vk::ImageUsageFlags draw_image_usages{};
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferSrc;
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferDst;
        draw_image_usages |= vk::ImageUsageFlagBits::eStorage;
        draw_image_usages |= vk::ImageUsageFlagBits::eColorAttachment;
        draw_image_usages |= vk::ImageUsageFlagBits::eSampled;
    }

    void LightingPass::InitDrawImage()
    {
        vk::Extent3D draw_image_extent{
            3840,
            2160,
            1,
        };

        // 设为16位浮点格式以获得更高的精度
        draw_image.format = vk::Format::eR16G16B16A16Sfloat;
        draw_image.extent = draw_image_extent;

        vk::ImageUsageFlags draw_image_usages{};
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferSrc;
        draw_image_usages |= vk::ImageUsageFlagBits::eTransferDst;
        draw_image_usages |= vk::ImageUsageFlagBits::eStorage;
        draw_image_usages |= vk::ImageUsageFlagBits::eColorAttachment;

        draw_image = m_rhi->CreateImage(draw_image_extent, draw_image.format, draw_image_usages);
    }

    void LightingPass::InitUnifromBuffers()
    {
        m_scene_uniform_buffer = m_rhi->CreateBuffer(sizeof(m_scene_uniform_data), vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu);
    }

    void LightingPass::InitDescriptors()
    {
        m_descriptors.resize(descriptor_count);

        // gbuffer
        {
            // 创建描述符布局
            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(1, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(2, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(3, vk::DescriptorType::eCombinedImageSampler)
                    .SetShaderStage(vk::ShaderStageFlagBits::eAllGraphics);

            m_descriptors[gbuffer_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            // 分配描述符集
            m_descriptors[gbuffer_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[gbuffer_descriptor].layout);

            auto writer =
                DescriptorWriter()
                    .WriteImage(0, gbuffer_base_color.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(1, gbuffer_world_normal.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(2, gbuffer_world_position.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
                    .WriteImage(3, gbuffer_metallic_roughness.image_view, m_shadowmap_sampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
            m_rhi->UpdateDescriptorSets(writer, m_descriptors[gbuffer_descriptor].set);
        }

        // scene
        {
            // 创建描述符布局
            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eUniformBuffer)
                    .AddBinding(1, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(2, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(3, vk::DescriptorType::eCombinedImageSampler)
                    .AddBinding(4, vk::DescriptorType::eCombinedImageSampler)
                    .SetShaderStage(vk::ShaderStageFlagBits::eAllGraphics);

            m_descriptors[scene_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            // 分配描述符集
            m_descriptors[scene_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[scene_descriptor].layout);
        }

        // skybox
        {
            DescriptorSetLayoutCreateInfo layout_info;

            auto layout_ci =
                DescriptorSetLayoutCreateInfo()
                    .AddBinding(0, vk::DescriptorType::eUniformBuffer)
                    .AddBinding(1, vk::DescriptorType::eCombinedImageSampler)
                    .SetShaderStage(vk::ShaderStageFlagBits::eAllGraphics);

            m_descriptors[skybox_descriptor].layout = m_rhi->CreateDescriptorSetLayout(layout_ci);

            m_descriptors[skybox_descriptor].set = m_rhi->AllocateDescriptorSets(m_descriptors[skybox_descriptor].layout);
        }
    }

    void LightingPass::InitPipelines()
    {
        m_pipelines.resize(pipeline_count);

        //-----------main-----------
        {
            //----------shader--------------
            vk::ShaderModule lighting_vert_shader = m_rhi->CreateShaderModule(LIGHTING_VERT_CODE);
            vk::ShaderModule lighting_frag_shader = m_rhi->CreateShaderModule(LIGHTING_FRAG_CODE);

            //-----------管线布局-------------
            std::vector<vk::DescriptorSetLayout> descriptor_set_layouts{
                m_descriptors[gbuffer_descriptor].layout,
                m_descriptors[scene_descriptor].layout,
            };

            auto push_constant_range =
                vk::PushConstantRange()
                    .setOffset(0)
                    .setSize(sizeof(m_skybox_push_constants))
                    .setStageFlags(vk::ShaderStageFlagBits::eVertex);

            auto pipeline_ci =
                vk::PipelineLayoutCreateInfo()
                    .setPushConstantRanges(push_constant_range)
                    .setSetLayouts(descriptor_set_layouts);
            m_pipelines[main_pipeline].layout = m_rhi->CreatePipelineLayout(pipeline_ci);

            //-----------创建管线----------
            auto render_pipeline_ci =
                RenderPipelineCreateInfo()
                    .SetPipelineLayout(m_pipelines[main_pipeline].layout)
                    .SetShaders(lighting_vert_shader, lighting_frag_shader)
                    .SetInputTopolygy(vk::PrimitiveTopology::eTriangleList)
                    .SetPolygonMode(vk::PolygonMode::eFill)
                    .SetCullMode(vk::CullModeFlagBits::eFront, vk::FrontFace::eCounterClockwise)
                    .SetMultisamplingNone()
                    .DisableBlending()
                    .EnableDepthTest(vk::True, vk::CompareOp::eGreaterOrEqual)
                    .SetColorAttachmentFormats({draw_image.format})
                    .SetDepthFormat(depth_image.format);

            m_pipelines[main_pipeline].pipeline = m_rhi->CreateRenderPipeline(render_pipeline_ci);

            //--------清除---------
            m_rhi->DestroyShaderModule(lighting_vert_shader);
            m_rhi->DestroyShaderModule(lighting_frag_shader);
        }

        //-----------skybox-----------
        {
            //----------shader--------------
            vk::ShaderModule skybox_vert_shader = m_rhi->CreateShaderModule(SKYBOX_VERT_CODE);
            vk::ShaderModule skybox_frag_shader = m_rhi->CreateShaderModule(SKYBOX_FRAG_CODE);

            //------------推送常量------------
            auto push_constant_range =
                vk::PushConstantRange()
                    .setOffset(0)
                    .setSize(sizeof(m_skybox_push_constants))
                    .setStageFlags(vk::ShaderStageFlagBits::eVertex);

            //-----------管线布局-------------
            std::vector<vk::DescriptorSetLayout> descriptor_set_layouts{
                m_descriptors[skybox_descriptor].layout,
            };

            auto pipeline_ci =
                vk::PipelineLayoutCreateInfo()
                    .setPushConstantRanges(push_constant_range)
                    .setSetLayouts(descriptor_set_layouts);
            m_pipelines[skybox_pipeline].layout = m_rhi->CreatePipelineLayout(pipeline_ci);

            //-----------创建管线----------
            auto render_pipeline_ci =
                RenderPipelineCreateInfo()
                    .SetPipelineLayout(m_pipelines[skybox_pipeline].layout)
                    .SetShaders(skybox_vert_shader, skybox_frag_shader)
                    .SetInputTopolygy(vk::PrimitiveTopology::eTriangleList)
                    .SetPolygonMode(vk::PolygonMode::eFill)
                    .SetCullMode(vk::CullModeFlagBits::eFront, vk::FrontFace::eCounterClockwise)
                    .SetMultisamplingNone()
                    .DisableBlending()
                    .DisableDepthTest()
                    .SetColorAttachmentFormat(draw_image.format)
                    .SetDepthFormat(depth_image.format);

            m_pipelines[skybox_pipeline].pipeline = m_rhi->CreateRenderPipeline(render_pipeline_ci);

            //--------清除---------
            m_rhi->DestroyShaderModule(skybox_vert_shader);
            m_rhi->DestroyShaderModule(skybox_frag_shader);
        }
    }

    void LightingPass::CopyToSwapchain()
    {
        // 将交换链布局转换为传输目标布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_rhi->GetCurrentSwapchainImage(),
                               vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eTransferDstOptimal);

        // 将渲染图像拷贝至交换链
        m_rhi->CopyImageToImage(m_rhi->GetCurrentCommandBuffer(),
                                draw_image.image,
                                m_rhi->GetCurrentSwapchainImage(),
                                m_draw_extent,
                                m_rhi->GetSwapChainExtent());
    }

    void LightingPass::UpdateUniformBuffer()
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

    void LightingPass::DrawGeometry()
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

        // 颜色附件
        auto color_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(draw_image.image_view)
                .setImageLayout(vk::ImageLayout::eGeneral)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setClearValue(color_clear_value);

        // 开始渲染
        auto render_info =
            vk::RenderingInfo()
                .setRenderArea(vk::Rect2D({0, 0}, m_draw_extent))
                .setLayerCount(1)
                .setColorAttachments(color_attachment)
                .setPDepthAttachment({});

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

        // skybox
        {
            cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelines[skybox_pipeline].layout, 0, m_descriptors[skybox_descriptor].set, {});

            cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[skybox_pipeline].pipeline);

            m_skybox_push_constants.vertex_buffer = m_render_scene->skybox->cube->vertex_buffer_address;

            cmd_buffer.pushConstants(m_pipelines[skybox_pipeline].layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(m_skybox_push_constants), &m_skybox_push_constants);

            // 绑定IBO
            cmd_buffer.bindIndexBuffer(m_render_scene->skybox->cube->index_buffer.buffer, 0, vk::IndexType::eUint32);

            cmd_buffer.drawIndexed(m_render_scene->skybox->cube->index_count, 1, 0, 0, 0);
        }

        // 绑定全局变量描述符
        cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].layout, 0, m_descriptors[gbuffer_descriptor].set, {});
        cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].layout, 1, m_descriptors[scene_descriptor].set, {});

        // 绑定管线
        cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipelines[main_pipeline].pipeline);

        m_push_constants.width  = m_rhi->GetSwapChainExtent().width;
        m_push_constants.height = m_rhi->GetSwapChainExtent().height;

        cmd_buffer.pushConstants(m_pipelines[main_pipeline].layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(m_push_constants), &m_push_constants);

        cmd_buffer.draw(3, 1, 0, 0);

        cmd_buffer.endRendering();
    }

} // namespace Yutrel