// #pragma once

// #include <vulkan/vulkan.hpp>

// #include "resource_manager.h"

// namespace Yutrel
// {
//     class RenderTarget
//     {
//     public:
//         struct CreateInfo
//         {
//             vk::Extent2D extent;
//             std::vector<vk::Format> color_formats;
//             vk::Format depth_format{vk::Format::eUndefined};
//         };

//     public:
//         RenderTarget() = delete;
//         RenderTarget(std::shared_ptr<class Renderer> renderer, const CreateInfo& info);
//         ~RenderTarget();

//         static std::unique_ptr<RenderTarget> create(std::shared_ptr<class Renderer> renderer, const CreateInfo& info);

//     private:
//         std::shared_ptr<Renderer> m_renderer;

//         std::vector<Image> m_color_images;
//         Image m_depth_image;
//         vk::ImageView m_depth_view;
//         std::vector<vk::DescriptorImageInfo> m_descriptors;
//         vk::Sampler m_sampler;

//         vk::Extent2D m_extent;
//         std::vector<vk::Format> m_color_formats;
//         vk::Format m_depth_format{vk::Format::eD16Unorm};
//         // std::vector<vk::DescriptorSet> m_descriptor_sets;
//     };
// } // namespace Yutrel