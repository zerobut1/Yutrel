#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

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
        VkSemaphore swapchain_semaphore, render_semaphore;
        VkFence render_fence;

        // 摄像机缓冲
        AllocatedBuffer camera_buffer;
        VkDescriptorSet global_descriptor;

        // 对象缓冲
        AllocatedBuffer object_buffer;
        VkDescriptorSet object_descriptor;
    };

    // 上传指令的结构
    struct UploadContext
    {
        VkFence upload_fence;
        VkCommandPool command_pool;
        VkCommandBuffer command_buffer;
    };

} // namespace Yutrel
