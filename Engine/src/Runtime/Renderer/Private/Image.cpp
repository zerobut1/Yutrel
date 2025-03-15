#include "Image.h"

namespace Yutrel::utils
{
    vk::ImageCreateInfo makeImage2DCreateInfo(const vk::Extent2D extent,
                                              vk::Format format /*= vk::Format::eR8G8B8A8Unorm*/,
                                              vk::ImageUsageFlags usage /*= vk::ImageUsageFlagBits::eSampled*/,
                                              bool b_mipmaps /*= false*/)
    {
        auto info =
            vk::ImageCreateInfo()
                .setImageType(vk::ImageType::e2D)
                .setFormat(format)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setMipLevels(b_mipmaps ? mipLevels(extent) : 1)
                .setArrayLayers(1)
                .setExtent({extent.width, extent.height, 1})
                .setUsage(usage |
                          vk::ImageUsageFlagBits::eTransferSrc |
                          vk::ImageUsageFlagBits::eTransferDst);

        return info;
    }

    vk::ImageViewCreateInfo makeImage2DViewCreateInfo(vk::Image image,
                                                      vk::Format format /*= vk::Format::eR8G8B8A8Unorm*/,
                                                      vk::ImageAspectFlags aspect_flags /*= vk::ImageAspectFlagBits::eColor*/,
                                                      uint32_t levels /*= vk::RemainingMipLevels*/,
                                                      const void* p_next /*= nullptr*/)
    {
        auto sub =
            vk::ImageSubresourceRange()
                .setAspectMask(aspect_flags)
                .setBaseMipLevel(0)
                .setLevelCount(levels)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        auto info =
            vk::ImageViewCreateInfo()
                .setPNext(p_next)
                .setImage(image)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(format)
                .setSubresourceRange(sub);

        return info;
    }
} // namespace Yutrel::utils