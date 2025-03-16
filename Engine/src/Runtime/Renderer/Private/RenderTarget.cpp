#include "RenderTarget.h"

#include "Image.h"
#include "Renderer.h"

namespace Yutrel
{
    RenderTarget::RenderTarget(std::shared_ptr<Renderer> renderer, const CreateInfo& info)
        : m_renderer(renderer)
    {
        auto resource_manager = renderer->getResourceManager();

        m_extent = info.extent;
        m_format = info.format;

        // image
        {
            vk::ImageUsageFlags usage =
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
            vk::ImageCreateInfo info = utils::makeImage2DCreateInfo(m_extent, m_format, usage);
            m_image                  = resource_manager->createImage(info);
        }
        // image view
        {
            vk::ImageViewCreateInfo info = utils::makeImage2DViewCreateInfo(m_image.image, m_format);
            m_descriptor.setImageView(resource_manager->createImageView(info));
        }
        // descriptor
        if (m_descriptor.sampler == nullptr)
        {
            auto info = vk::SamplerCreateInfo();
            m_descriptor.setSampler(resource_manager->createSampler(info));
        }
        // layout
        {
            auto cmd_buffer = m_renderer->beginSingleTimeCommandBuffer();
            renderer->transitionImageLayout(cmd_buffer,
                                            m_image.image,
                                            vk::ImageLayout::eUndefined,
                                            info.layout);
            m_descriptor.setImageLayout(info.layout);

            // clear
            vk::ClearColorValue clear_value = {0.0f, 0.0f, 0.0f, 0.0f};
            vk::ImageSubresourceRange range = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
            cmd_buffer.clearColorImage(m_image.image, info.layout, clear_value, range);

            m_renderer->endSingleTimeCommandBuffer(cmd_buffer);
        }
    }

    RenderTarget::~RenderTarget()
    {
    }

    std::unique_ptr<RenderTarget> RenderTarget::create(std::shared_ptr<class Renderer> renderer, const CreateInfo& info)
    {
        return std::make_unique<RenderTarget>(renderer, info);
    }

    void RenderTarget::transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::ImageLayout layout)
    {
        m_renderer->transitionImageLayout(cmd_buffer,
                                          m_image.image,
                                          m_descriptor.imageLayout,
                                          layout);

        m_descriptor.setImageLayout(layout);
    }

} // namespace Yutrel