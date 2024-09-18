#pragma once

#include <vulkan/vulkan.hpp>

namespace Yutrel::utils
{
    inline uint32_t mipLevels(vk::Extent2D extent)
    {
        return static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1;
    }

    vk::ImageCreateInfo makeImage2DCreateInfo(const vk::Extent2D extent,
                                              vk::Format format         = vk::Format::eR8G8B8A8Unorm,
                                              vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled,
                                              bool b_mipmaps            = false);

    vk::ImageViewCreateInfo makeImage2DViewCreateInfo(vk::Image image,
                                                      vk::Format format                 = vk::Format::eR8G8B8A8Unorm,
                                                      vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor,
                                                      uint32_t levels                   = vk::RemainingMipLevels,
                                                      const void* p_next                = nullptr);
} // namespace Yutrel::utils