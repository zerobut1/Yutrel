#pragma once

#include <vulkan/vulkan.hpp>

#include "ResourceManager.h"

namespace Yutrel
{
    class Renderer;

    class RenderTarget
    {
    public:
        struct CreateInfo
        {
            vk::Extent2D extent;
            vk::Format format;
            vk::ImageLayout layout;
        };

    public:
        RenderTarget() = delete;
        explicit RenderTarget(Renderer* renderer, const CreateInfo& info);
        ~RenderTarget();

        static std::unique_ptr<RenderTarget> create(Renderer* renderer, const CreateInfo& info);

        void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::ImageLayout layout);

        vk::Image getImage() const { return m_image.image; }
        vk::Extent2D getExtent() const { return m_extent; }
        vk::Format getFormat() const { return m_format; }
        vk::ImageView getImageView() const { return m_descriptor.imageView; }
        vk::ImageLayout getLayout() const { return m_descriptor.imageLayout; }
        vk::Sampler getSampler() const { return m_descriptor.sampler; }
        vk::DescriptorImageInfo getDescriptorImageInfo() const { return m_descriptor; }

    private:
        Renderer* m_renderer;

        Image m_image;
        vk::DescriptorImageInfo m_descriptor;

        vk::Extent2D m_extent;
        vk::Format m_format;
        // vk::Sampler m_sampler;
        // std::vector<vk::DescriptorSet> m_descriptor_sets;
    };
} // namespace Yutrel