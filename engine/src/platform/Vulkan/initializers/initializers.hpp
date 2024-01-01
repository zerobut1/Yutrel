#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    namespace vkinit
    {
        // 指令池创建信息
        VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags flags);

        // 指令缓冲分配信息
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level);

        // 栅栏创建信息
        VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags);

        // 信号量创建信息
        VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags);

        // 帧缓冲创建信息
        VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass render_pass, VkExtent2D extent);

        // 管线布局创建信息
        VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();

        // 渲染管线着色器阶段创建信息
        VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader_module);

        // 顶点输入阶段创建信息
        VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo();

        // 图元装配创建信息
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology);

        // 固定功能的光栅化阶段创建信息
        VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygon_mode);

        // MSAA
        VkPipelineMultisampleStateCreateInfo MultiSamplingStateCreateInfo();

        // 颜色混合
        VkPipelineColorBlendAttachmentState ColorBlendAttachmentState();

        // 命令缓冲区开始信息
        VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags);

        // renderpass开始信息
        VkRenderPassBeginInfo RenderPassBeginInfo(VkRenderPass render_pass, VkExtent2D window_extent, VkFramebuffer framebuffer);

        // 提交信息
        VkSubmitInfo SubmitInfo(VkCommandBuffer* cmd_buffer);

        // 展示信息
        VkPresentInfoKHR PresentInfo();

        // 图像子资源范围
        VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspect_mask);

        // 信号量提交信息
        VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore);

        // 指令缓冲提交信息
        VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd_buffer);

        // 提交信息2
        VkSubmitInfo2 SubmitInfo2(VkCommandBufferSubmitInfo* cmd_buffer_info, VkSemaphoreSubmitInfo* signal_semaphore_info, VkSemaphoreSubmitInfo* wait_semaphore_info);

        // 图像创建信息
        VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent);

        // 图像视图创建信息
        VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags);

        // 渲染附件信息
        VkRenderingAttachmentInfo AttachmentInfo(VkImageView view, VkClearValue* clear, VkImageLayout layout);

        // 渲染信息
        VkRenderingInfo RenderingInfo(VkExtent2D render_extent, VkRenderingAttachmentInfo* color_attachment, VkRenderingAttachmentInfo* depth_attachment);

    } // namespace vkinit

} // namespace Yutrel