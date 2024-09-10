// #include "renderer.h"

// #include "Core/log.h"
// #include "Function/Render/context.h"
// #include "Function/Render/frame.h"
// #include "Function/Render/resource_manager.h"
// #include "Function/Render/swapchain.h"

// namespace Yutrel
// {
//     Renderer::Renderer(const RendererCreateInfo& info)
//     {
//         LOG_INFO("Create Vulkan Renderer");
//         init(info);
//     }

//     Renderer::~Renderer()
//     {
//         shutdown();
//     }

//     void Renderer::init(const RendererCreateInfo& info)
//     {
//         //--------context------------
//         ContextCreateInfo context_ci{};
//         context_ci.window             = info.window;
//         context_ci.device_features    = info.device_features;
//         context_ci.device_features_12 = info.device_features_12;
//         context_ci.device_features_13 = info.device_features_13;

//         m_context = std::make_shared<Context>(context_ci);

//         //---------交换链------------
//         SwapchianCreateInfo swapchain_ci{};
//         swapchain_ci.context = m_context;
//         swapchain_ci.width   = info.width;
//         swapchain_ci.height  = info.height;

//         m_swapchain = std::make_shared<Swapchain>(swapchain_ci);

//         //----------帧数据--------------
//         for (auto& frame : m_frames)
//         {
//             frame = std::make_shared<Frame>(m_context);
//         }

//         //--------资源管理
//         m_resource_manager = std::make_shared<ResourceManager>(m_context);

//         //---------单次指令池-------------
//         auto cmd_pool_ci =
//             vk::CommandPoolCreateInfo()
//                 .setQueueFamilyIndex(m_context->getGraphicsQueueIndex())
//                 .setFlags({});

//         m_cmd_pool = m_context->getDevice().createCommandPool(cmd_pool_ci);
//     }

//     void Renderer::shutdown()
//     {
//         auto device = m_context->getDevice();

//         device.waitIdle();

//         device.destroy(m_cmd_pool);

//         m_resource_manager.reset();

//         for (auto frame : m_frames)
//         {
//             frame.reset();
//         }

//         m_swapchain.reset();

//         m_context.reset();
//     }

//     vk::CommandBuffer Renderer::prepareBeforeRender()
//     {
//         auto cur_frame = getCurrentFrame();

//         // fence
//         cur_frame->waitForFence();

//         // 获取交换链图像
//         m_swapchain->acquireNextImage(cur_frame->getAvailableForRenderSemaphore());

//         // begin cmd buffer
//         return cur_frame->beginCommandBuffer();
//     }

//     void Renderer::submitRendering()
//     {
//         auto cur_frame = getCurrentFrame();

//         // 终止指令缓冲
//         cur_frame->endCommandBuffer();

//         // 提交指令
//         cur_frame->submitCommandBuffer();
//     }

//     void Renderer::framePresent()
//     {
//         auto cur_frame = getCurrentFrame();

//         m_swapchain->present(cur_frame->getFinishedForPresentationSemaphore());

//         m_frame_count++;
//     }

//     void Renderer::transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout)
//     {
//         vk::ImageAspectFlags aspect_mask = vk::ImageAspectFlagBits::eColor;

//         auto image_barrier =
//             vk::ImageMemoryBarrier2()
//                 .setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
//                 .setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
//                 .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
//                 .setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead)
//                 .setOldLayout(cur_layout)
//                 .setNewLayout(new_layout)
//                 .setSubresourceRange(vk::ImageSubresourceRange(aspect_mask, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers))
//                 .setImage(image);

//         auto dependency_info =
//             vk::DependencyInfo()
//                 .setImageMemoryBarriers(image_barrier);

//         cmd_buffer.pipelineBarrier2(dependency_info);
//     }

// } // namespace Yutrel