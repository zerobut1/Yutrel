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

} // namespace Yutrel
