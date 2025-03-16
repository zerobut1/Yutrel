#pragma once

#include <vulkan/vulkan.hpp>

#include "ResourceManager.h"

namespace Yutrel
{
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
        RenderTarget(std::shared_ptr<class Renderer> renderer, const CreateInfo& info);
        ~RenderTarget();

        static std::unique_ptr<RenderTarget> create(std::shared_ptr<class Renderer> renderer, const CreateInfo& info);

        void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::ImageLayout layout);

        vk::Image getImage() const { return m_image.image; }
        vk::Extent2D getExtent() const { return m_extent; }
        vk::ImageLayout getLayout() const { return m_descriptor.imageLayout; }

    private:
        std::shared_ptr<Renderer> m_renderer;

        Image m_image;
        vk::DescriptorImageInfo m_descriptor;

        vk::Extent2D m_extent;
        vk::Format m_format;
        // vk::Sampler m_sampler;
        // std::vector<vk::DescriptorSet> m_descriptor_sets;
    };
} // namespace Yutrel