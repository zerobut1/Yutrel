#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Yutrel
{
    // 分配缓冲区
    struct AllocatedBuffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
    };

    // 分配图像
    struct AllocatedImage
    {
        VkImage image;
        VmaAllocation allocation;
    };

    // 帧数据
    struct FrameData
    {
        // 指令
        VkCommandPool command_pool;
        VkCommandBuffer main_command_buffer;

        // 同步
        VkSemaphore finished_for_presentation_semaphore, available_for_render_semaphore;
        VkFence render_fence;

        // // 摄像机缓冲
        // AllocatedBuffer camera_buffer;
        // VkDescriptorSet global_descriptor;

        // // 对象缓冲
        // AllocatedBuffer object_buffer;
        // VkDescriptorSet object_descriptor;
    };

    // 交换链信息
    struct RHISwapChainDesc
    {
        VkExtent2D extent;
        VkFormat image_format;
        VkViewport viewport;
        VkRect2D scissor;
        std::vector<VkImageView>* image_views;
    };

    // 图形管线创建信息
    struct RHIGraphicsPipelineCreateInfo
    {
        // 渲染管线布局
        VkPipelineLayout pipeline_layout;

        // 着色器阶段
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

        // 顶点输入阶段
        VkPipelineVertexInputStateCreateInfo vertex_input;

        // 图元装配
        VkPipelineInputAssemblyStateCreateInfo input_assembly;

        // 视口阶段
        VkPipelineViewportStateCreateInfo viewport_state;

        // 光栅化阶段
        VkPipelineRasterizationStateCreateInfo rasterizer;

        // MSAA
        VkPipelineMultisampleStateCreateInfo multisampling;

        // 颜色混合
        VkPipelineColorBlendStateCreateInfo color_blend;

        // 渲染pass
        VkRenderPass render_pass;

        // subpass
        uint32_t subpass;
    };

} // namespace Yutrel
