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
        std::vector<VkImageView>* image_views;
    };

} // namespace Yutrel
