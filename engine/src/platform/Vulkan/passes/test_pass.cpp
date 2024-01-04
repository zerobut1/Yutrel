#include "yutrel_pch.hpp"

#include "test_pass.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/mesh/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

namespace Yutrel
{
    void TestPass::Init(RenderPassInitInfo* info)
    {
        InitDrawImage();

        InitDepthImage();

        InitDescriptors();

        m_pipelines.resize(pipelines::pipeline_count);

        InitComputePipeline();

        InitTrianglePipeline();
    }

    void TestPass::PreparePassData(Ref<struct RenderData> render_data)
    {
        m_render_data = render_data;
    }

    void TestPass::DrawForward()
    {
        PrepareDrawImage();

        //--------绘制------------
        // 清除图像
        DrawBackground();

        // 图像格式转换为颜色缓冲
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_GENERAL,
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

    void TestPass::InitDrawImage()
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

        m_rhi->CreateImage(&draw_image_create_info, &draw_image_alloc_info, &m_draw_image);

        // 创建图像视图
        VkImageViewCreateInfo draw_image_view_info = vkinit::ImageViewCreateInfo(m_draw_image.image_format, m_draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);

        m_rhi->CreateImageView(&draw_image_view_info, &m_draw_image);
    }

    void TestPass::InitDepthImage()
    {
        m_depth_image.image_format = VK_FORMAT_D32_SFLOAT;
        m_depth_image.image_extent = m_draw_image.image_extent;

        VkImageUsageFlags depth_image_usages{};
        depth_image_usages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkImageCreateInfo depth_image_info = vkinit::ImageCreateInfo(m_depth_image.image_format, depth_image_usages, m_depth_image.image_extent);

        VmaAllocationCreateInfo depth_image_alloc_info{};
        depth_image_alloc_info.usage         = VMA_MEMORY_USAGE_GPU_ONLY;
        depth_image_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_rhi->CreateImage(&depth_image_info, &depth_image_alloc_info, &m_depth_image);

        VkImageViewCreateInfo depth_view_info = vkinit::ImageViewCreateInfo(m_depth_image.image_format, m_depth_image.image, VK_IMAGE_ASPECT_DEPTH_BIT);

        m_rhi->CreateImageView(&depth_view_info, &m_depth_image);
    }

    void TestPass::InitDescriptors()
    {
        m_descriptor_infos.resize(descriptor_count);

        // 计算着色器
        {
            // 创建描述符布局
            RHIDescriptorLayoutCreateInfo layout_info{};
            layout_info.shader_stages = VK_SHADER_STAGE_COMPUTE_BIT;
            layout_info.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

            m_rhi->CreateDescriptorLayout(layout_info, &m_descriptor_infos[compute_descriptor].layout);

            // 分配描述符集
            m_rhi->AllocateDescriptorSets(m_descriptor_infos[compute_descriptor].layout, &m_descriptor_infos[compute_descriptor].set);

            // 写描述符集
            DescriptorWriter writer;
            writer.WriteImage(0, m_draw_image.image_view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

            m_rhi->UpdateDescriptorSets(writer, m_descriptor_infos[compute_descriptor].set);
        }

        // 场景信息
        {
            RHIDescriptorLayoutCreateInfo layout_info{};
            layout_info.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            layout_info.shader_stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

            m_rhi->CreateDescriptorLayout(layout_info, &m_descriptor_infos[scene_descriptor].layout);
        }
    }

    void TestPass::InitComputePipeline()
    {
        // layout
        VkPipelineLayoutCreateInfo compute_layout{};
        compute_layout.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        compute_layout.pNext          = nullptr;
        compute_layout.pSetLayouts    = &m_descriptor_infos[compute_descriptor].layout;
        compute_layout.setLayoutCount = 1;

        VkPushConstantRange push_constant{};
        push_constant.offset     = 0;
        push_constant.size       = sizeof(ComputePushConstants);
        push_constant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        compute_layout.pushConstantRangeCount = 1;
        compute_layout.pPushConstantRanges    = &push_constant;

        m_rhi->CreatePipelineLayout(&compute_layout, &m_pipelines[compute_pipeline].layout);

        // shader
        // clang-format off
        std::vector<unsigned char> compute_code{
            #include "gradient.comp.spv.h"
        };
        // clang-format on
        VkShaderModule compute_shader;
        if (!m_rhi->CreateShaderModule(compute_code, &compute_shader))
        {
            LOG_ERROR("Failed to create compute shader");
        }

        VkPipelineShaderStageCreateInfo stageinfo{};
        stageinfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageinfo.pNext  = nullptr;
        stageinfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
        stageinfo.module = compute_shader;
        stageinfo.pName  = "main";

        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.pNext  = nullptr;
        computePipelineCreateInfo.layout = m_pipelines[compute_pipeline].layout;
        computePipelineCreateInfo.stage  = stageinfo;

        m_rhi->CreateComputePipelines(VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_pipelines[compute_pipeline].pipeline);

        // 清除着色器模块
        m_rhi->DestroyShaderModule(compute_shader);
    }

    void TestPass::InitTrianglePipeline()
    {
        //----------shader--------------
        // clang-format off
        std::vector<unsigned char> triangle_vert_code{
            #include "triangle.vert.spv.h"
        };
        std::vector<unsigned char> triangle_frag_code{
            #include "triangle.frag.spv.h"
        };
        // clang-format on
        VkShaderModule triangle_vert_shader;
        if (!m_rhi->CreateShaderModule(triangle_vert_code, &triangle_vert_shader))
        {
            LOG_ERROR("Failed to create vertex shader");
        }
        VkShaderModule triangle_frag_shader;
        if (!m_rhi->CreateShaderModule(triangle_frag_code, &triangle_frag_shader))
        {
            LOG_ERROR("Failed to create fragment shader");
        }

        //------------推送常量------------
        VkPushConstantRange buffer_range{};
        buffer_range.offset     = 0;
        buffer_range.size       = sizeof(GPUDrawPushConstants);
        buffer_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        //------------layout-------------
        VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::PipelineLayoutCreateInfo();
        pipeline_layout_info.pushConstantRangeCount     = 1;
        pipeline_layout_info.pPushConstantRanges        = &buffer_range;

        m_rhi->CreatePipelineLayout(&pipeline_layout_info, &m_pipelines[pipelines::triangle_pipeline].layout);

        //-----------创建管线----------
        RHIDynamicPipelineCreateInfo pipeline_create_info;
        pipeline_create_info.Clear();
        pipeline_create_info.pipeline_layout = m_pipelines[pipelines::triangle_pipeline].layout;
        pipeline_create_info.SetShaders(triangle_vert_shader, triangle_frag_shader);
        pipeline_create_info.SetInputTopolygy(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipeline_create_info.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipeline_create_info.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        pipeline_create_info.SetMultisamplingNone();
        pipeline_create_info.EnableBlendingAlphablend();
        pipeline_create_info.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
        pipeline_create_info.SetColorAttachmentFormat(m_draw_image.image_format);
        pipeline_create_info.SetDepthFormat(m_depth_image.image_format);

        m_rhi->CreateDynamicPipelines(pipeline_create_info, &m_pipelines[pipelines::triangle_pipeline].pipeline);

        //--------清除---------
        m_rhi->DestroyShaderModule(triangle_vert_shader);
        m_rhi->DestroyShaderModule(triangle_frag_shader);
    }

    void TestPass::PrepareDrawImage()
    {
        // 设置渲染图像范围
        m_draw_extent.width  = std::min(m_rhi->GetSwapChainInfo().extent.width, m_draw_image.image_extent.width) * m_render_scale;
        m_draw_extent.height = std::min(m_rhi->GetSwapChainInfo().extent.height, m_draw_image.image_extent.height) * m_render_scale;

        // 将渲染图像布局转换为通用布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_GENERAL);
    }

    void TestPass::CopyToSwapchain()
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

    void TestPass::DrawBackground()
    {
        auto cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelines[compute_pipeline].pipeline);

        vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelines[compute_pipeline].layout, 0, 1, &m_descriptor_infos[compute_descriptor].set, 0, nullptr);

        ComputePushConstants push_constant;
        push_constant.data1 = m_render_data->background.data1;
        push_constant.data2 = m_render_data->background.data2;
        push_constant.data3 = m_render_data->background.data3;
        push_constant.data4 = m_render_data->background.data4;

        vkCmdPushConstants(cmd_buffer, m_pipelines[compute_pipeline].layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &push_constant);

        vkCmdDispatch(cmd_buffer, std::ceil(m_draw_extent.width / 16.0), std::ceil(m_draw_extent.height / 16.0), 1);
    }

    void TestPass::DrawGeometry()
    {
        //------------描述符集----------
        AllocatedBuffer gpu_scene_data_buffer = m_rhi->CreateBuffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        m_rhi->GetCurrentFrame().deletion_queue.PushFunction(
            [=]()
            {
                m_rhi->DestroyBuffer(gpu_scene_data_buffer);
            });

        // 写缓冲
        GPUSceneData* scene_uniform_data = reinterpret_cast<GPUSceneData*>(gpu_scene_data_buffer.info.pMappedData);
        *scene_uniform_data              = scene_data;

        // 创建描述符集
        VkDescriptorSet global_descriptor = m_rhi->GetCurrentFrame().descriptors.Allocate(m_descriptor_infos[scene_descriptor].layout);

        DescriptorWriter writer;
        writer.WriteBuffer(0, gpu_scene_data_buffer.buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        m_rhi->UpdateDescriptorSets(writer, global_descriptor);

        //---------渲染信息----------------

        // 颜色附件
        VkRenderingAttachmentInfo color_attachment = vkinit::AttachmentInfo(m_draw_image.image_view, nullptr, VK_IMAGE_LAYOUT_GENERAL);

        // 深度附件
        VkRenderingAttachmentInfo depth_attachment = vkinit::DepthAttachmentInfo(m_depth_image.image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        // 开始渲染
        VkRenderingInfo render_info = vkinit::RenderingInfo(m_draw_extent, &color_attachment, &depth_attachment);
        vkCmdBeginRendering(m_rhi->GetCurrentCommandBuffer(), &render_info);

        // 绑定三角形管线
        vkCmdBindPipeline(m_rhi->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[pipelines::triangle_pipeline].pipeline);

        // 设定viewport
        VkViewport viewport{};
        viewport.x        = 0;
        viewport.y        = 0;
        viewport.width    = m_draw_extent.width;
        viewport.height   = m_draw_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_rhi->GetCurrentCommandBuffer(), 0, 1, &viewport);

        // 设定scissor
        VkRect2D scissor{};
        scissor.offset.x      = 0;
        scissor.offset.y      = 0;
        scissor.extent.width  = m_draw_extent.width;
        scissor.extent.height = m_draw_extent.height;
        vkCmdSetScissor(m_rhi->GetCurrentCommandBuffer(), 0, 1, &scissor);

        // 推送常量
        // 将模型矩阵和顶点的设备地址传入
        GPUDrawPushConstants push_constants;

        // MVP矩阵
        // glm::mat4 view = glm::translate(glm::vec3{-2.0f, 0.0f, 2.0f});
        // glm::mat4 view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, -5.0f});
        glm::mat4 view       = glm::lookAt(glm::vec3(2.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)m_draw_extent.width / (float)m_draw_extent.height, 10000.0f, 0.1f);
        projection[1][1] *= -1;
        push_constants.world_matrix = projection * view;

        push_constants.vertex_buffer = m_render_data->pbrs[0]->mesh.gpu_buffers->vertex_buffer_address;

        vkCmdPushConstants(m_rhi->GetCurrentCommandBuffer(), m_pipelines[pipelines::triangle_pipeline].layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);

        vkCmdBindIndexBuffer(m_rhi->GetCurrentCommandBuffer(), m_render_data->pbrs[0]->mesh.gpu_buffers->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(m_rhi->GetCurrentCommandBuffer(), static_cast<uint32_t>(m_render_data->pbrs[0]->mesh.indices->size()), 1, 0, 0, 0);

        vkCmdEndRendering(m_rhi->GetCurrentCommandBuffer());
    }

} // namespace Yutrel