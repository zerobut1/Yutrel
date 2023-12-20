#include "yutrel_pch.hpp"

#include "vulkan_utils.hpp"

namespace Yutrel
{
    namespace vkutil
    {
        AllocatedImage CreateImage(VmaAllocator allocator,
                                   uint32_t image_width,
                                   uint32_t image_height,
                                   VkFormat format,
                                   VkImageTiling image_tiling,
                                   VkImageUsageFlags image_usage_flags,
                                   VkMemoryPropertyFlags memory_property_flags,
                                   VkImageCreateFlags image_create_flags,
                                   uint32_t array_layers,
                                   uint32_t miplevels)
        {
            AllocatedImage new_image;

            VkImageCreateInfo image_create_info{};
            image_create_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_create_info.flags         = image_create_flags;
            image_create_info.imageType     = VK_IMAGE_TYPE_2D;
            image_create_info.extent.width  = image_width;
            image_create_info.extent.height = image_height;
            image_create_info.extent.depth  = 1;
            image_create_info.mipLevels     = miplevels;
            image_create_info.arrayLayers   = array_layers;
            image_create_info.format        = format;
            image_create_info.tiling        = image_tiling;
            image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_create_info.usage         = image_usage_flags;
            image_create_info.samples       = VK_SAMPLE_COUNT_1_BIT;
            image_create_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo alloc_info{};
            alloc_info.usage         = VMA_MEMORY_USAGE_GPU_ONLY;
            alloc_info.requiredFlags = memory_property_flags;

            YUTREL_ASSERT(vmaCreateImage(allocator, &image_create_info, &alloc_info, &new_image.image, &new_image.allocation, nullptr) == VK_SUCCESS, "Failed to create image");

            return new_image;
        }

        VkImageView CreateImageView(VkDevice device,
                                    VkImage& image,
                                    VkFormat format,
                                    VkImageAspectFlags image_aspect_flags,
                                    VkImageViewType view_type,
                                    uint32_t layout_count,
                                    uint32_t miplevels)
        {
            VkImageViewCreateInfo image_view_create_info{};
            image_view_create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.image                           = image;
            image_view_create_info.viewType                        = view_type;
            image_view_create_info.format                          = format;
            image_view_create_info.subresourceRange.aspectMask     = image_aspect_flags;
            image_view_create_info.subresourceRange.baseMipLevel   = 0;
            image_view_create_info.subresourceRange.levelCount     = miplevels;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount     = layout_count;

            VkImageView image_view;

            YUTREL_ASSERT(vkCreateImageView(device, &image_view_create_info, nullptr, &image_view) == VK_SUCCESS, "Failed to create image view");

            return image_view;
        }

    } // namespace vkutil
} // namespace Yutrel