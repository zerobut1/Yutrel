// #include "render_target.h"

// #include "log.h"
// #include "image.h"
// #include "renderer.h"

// namespace Yutrel
// {
//     RenderTarget::RenderTarget(std::shared_ptr<Renderer> renderer, const CreateInfo& info)
//         : m_renderer(renderer)
//     {
//         YUTREL_ASSERT(m_color_formats.empty(), "");

//         auto resource_manager = renderer->getResourceManager();

//         m_extent        = info.extent;
//         m_color_formats = info.color_formats;
//         m_depth_format  = info.depth_format;

//         vk::ImageLayout layout{vk::ImageLayout::eGeneral};

//         auto num_color = static_cast<uint32_t>(m_color_formats.size());

//         m_color_images.reserve(num_color);
//         m_descriptors.reserve(num_color);

//         for (size_t color_index = 0; color_index < num_color; color_index++)
//         {
//             // color
//             {
//                 vk::ImageUsageFlags usage =
//                     vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
//                 vk::ImageCreateInfo info    = utils::makeImage2DCreateInfo(m_extent, m_color_formats[color_index], usage);
//                 m_color_images[color_index] = resource_manager->createImage(info);
//             }
//             // color view
//             {
//                 vk::ImageViewCreateInfo info = utils::makeImage2DViewCreateInfo(m_color_images[color_index].image, m_color_formats[color_index]);
//                 m_descriptors[color_index].setImageView(resource_manager->createImageView(info));
//             }
//             // descriptor
//             if (m_descriptors[color_index].sampler == nullptr)
//             {
//                 auto info = vk::SamplerCreateInfo();
//                 m_descriptors[color_index].setSampler(resource_manager->createSampler(info));
//             }
//         }

//         // 深度缓冲
//         if (m_depth_format != vk::Format::eUndefined)
//         {
//             vk::ImageCreateInfo image_ci = utils::makeImage2DCreateInfo(m_extent,
//                                                                         m_depth_format,
//                                                                         vk::ImageUsageFlagBits::eDepthStencilAttachment |
//                                                                             vk::ImageUsageFlagBits::eSampled);
//             m_depth_image                = resource_manager->createImage(image_ci);

//             vk::ImageViewCreateInfo view_ci =
//                 utils::makeImage2DViewCreateInfo(m_depth_image.image, m_depth_format)
//                     .setSubresourceRange({vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});
//             m_depth_view = resource_manager->createImageView(view_ci);
//         }

//         {
//             auto cmd_buffer = m_renderer->beginSingleTimeCommandBuffer();
//             for (size_t color_index = 0; color_index < num_color; color_index++)
//             {
//                 // 转换layout
//                 renderer->transitionImageLayout(cmd_buffer,
//                                                 m_color_images[color_index].image,
//                                                 vk::ImageLayout::eUndefined,
//                                                 layout);
//                 m_descriptors[color_index].setImageLayout(layout);
//                 // clean
//                 vk::ClearColorValue clear_value = {0.0f, 0.0f, 0.0f, 0.0f};
//                 vk::ImageSubresourceRange range = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
//                 cmd_buffer.clearColorImage(m_color_images[color_index].image, layout, clear_value, range);
//             }
//             m_renderer->endSingleTimeCommandBuffer(cmd_buffer);
//         }
//     }

//     RenderTarget::~RenderTarget()
//     {
//         m_color_images.clear();
//         m_descriptors.clear();
//         m_color_formats.clear();
//     }

//     std::unique_ptr<RenderTarget> RenderTarget::create(std::shared_ptr<class Renderer> renderer, const CreateInfo& info)
//     {
//         return std::make_unique<RenderTarget>(renderer, info);
//     }

// } // namespace Yutrel