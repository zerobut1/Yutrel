#include "yutrel_pch.hpp"

#include "test_pass.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

namespace Yutrel
{
    void TestPass::Init(RenderPassInitInfo* info)
    {
        InitDrawImage();

        InitDescriptors();

        InitBackgroundPipeline();
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
        //-----------------------

        CopyToSwapchain();
    }

    void TestPass::InitDrawImage()
    {
        //----------创建渲染目标图像---------------
        auto swapchain_extent = m_rhi->GetSwapChainInfo().extent;
        VkExtent3D draw_image_extent{
            swapchain_extent.width,
            swapchain_extent.height,
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

        YUTREL_ASSERT(m_rhi->CreateImage(&draw_image_create_info, &draw_image_alloc_info, &m_draw_image), "Failed to create draw image");

        // 创建图像视图
        VkImageViewCreateInfo draw_image_view_info = vkinit::ImageViewCreateInfo(m_draw_image.image_format, m_draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);

        YUTREL_ASSERT(m_rhi->CreateImageView(&draw_image_view_info, &m_draw_image), "Failed to create draw image view");
    }

    void TestPass::InitDescriptors()
    {
        m_descriptor_infos.resize(1);

        // 创建描述符布局
        RHIDescriptorLayoutCreateInfo layout_info{};
        layout_info.shader_stages = VK_SHADER_STAGE_COMPUTE_BIT;
        layout_info.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

        YUTREL_ASSERT(m_rhi->CreateDescriptorLayout(layout_info, &m_descriptor_infos[0].layout), "Failed to create descriptor layout");

        // 分配描述符集
        YUTREL_ASSERT(m_rhi->AllocateDescriptorSets(m_descriptor_infos[0].layout, &m_descriptor_infos[0].set), "Failed to allocate descriptor set");

        // 设置写描述符集信息
        VkDescriptorImageInfo img_info{};
        img_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        img_info.imageView   = m_draw_image.image_view;

        VkWriteDescriptorSet draw_image_write{};
        draw_image_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        draw_image_write.pNext = nullptr;

        draw_image_write.dstBinding      = 0;
        draw_image_write.dstSet          = m_descriptor_infos[0].set;
        draw_image_write.descriptorCount = 1;
        draw_image_write.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        draw_image_write.pImageInfo      = &img_info;

        // 更新描述符集
        m_rhi->UpdateDescriptorSets(1, &draw_image_write, 0, nullptr);
    }

    void TestPass::InitBackgroundPipeline()
    {
        m_pipelines.resize(1);

        // layout
        VkPipelineLayoutCreateInfo computeLayout{};
        computeLayout.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        computeLayout.pNext          = nullptr;
        computeLayout.pSetLayouts    = &m_descriptor_infos[0].layout;
        computeLayout.setLayoutCount = 1;

        YUTREL_ASSERT(m_rhi->CreatePipelineLayout(&computeLayout, &m_pipelines[0].layout), "Failed to create pipeline layout");

        // shader
        // clang-format off
        std::vector<unsigned char> compute_code{
            #include "gradient.comp.spv.h"
        };
        // clang-format on
        VkShaderModule compute_shader;
        m_rhi->CreateShaderModule(compute_code, &compute_shader);

        VkPipelineShaderStageCreateInfo stageinfo{};
        stageinfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageinfo.pNext  = nullptr;
        stageinfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
        stageinfo.module = compute_shader;
        stageinfo.pName  = "main";

        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.pNext  = nullptr;
        computePipelineCreateInfo.layout = m_pipelines[0].layout;
        computePipelineCreateInfo.stage  = stageinfo;

        m_rhi->CreateComputePipelines(VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_pipelines[0].pipeline);

        // 清除着色器模块
        m_rhi->DestroyShaderModule(compute_shader);
    }

    void TestPass::PrepareDrawImage()
    {
        // 设置渲染图像范围
        m_draw_extent.width  = m_draw_image.image_extent.width;
        m_draw_extent.height = m_draw_image.image_extent.height;

        // 将渲染图像布局转换为通用布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_GENERAL);
    }

    void TestPass::CopyToSwapchain()
    {
        // 将渲染图像布局转换为传输源布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_draw_image.image,
                               VK_IMAGE_LAYOUT_GENERAL,
                               VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

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

        // 将交换链布局转换为呈现布局
        m_rhi->TransitionImage(m_rhi->GetCurrentCommandBuffer(),
                               m_rhi->GetCurrentSwapchainImage(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

    void TestPass::DrawBackground()
    {
        vkCmdBindPipeline(m_rhi->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelines[0].pipeline);

        vkCmdBindDescriptorSets(m_rhi->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelines[0].layout, 0, 1, &m_descriptor_infos[0].set, 0, nullptr);

        vkCmdDispatch(m_rhi->GetCurrentCommandBuffer(), std::ceil(m_draw_extent.width / 16.0), std::ceil(m_draw_extent.height / 16.0), 1);
    }

} // namespace Yutrel