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

    } // namespace vkinit

} // namespace Yutrel