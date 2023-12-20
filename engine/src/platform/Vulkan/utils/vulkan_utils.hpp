#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    namespace vkutil
    {
        // 创建GPU图像
        AllocatedImage CreateImage(VmaAllocator allocator,
                                   uint32_t image_width,
                                   uint32_t image_height,
                                   VkFormat format,
                                   VkImageTiling image_tiling,
                                   VkImageUsageFlags image_usage_flags,
                                   VkMemoryPropertyFlags memory_property_flags,
                                   VkImageCreateFlags image_create_flags,
                                   uint32_t array_layers,
                                   uint32_t miplevels);

        // 创建图像视图
        VkImageView CreateImageView(VkDevice device,
                                    VkImage& image,
                                    VkFormat format,
                                    VkImageAspectFlags image_aspect_flags,
                                    VkImageViewType view_type,
                                    uint32_t layout_count,
                                    uint32_t miplevels);
    } // namespace vkutil
} // namespace Yutrel