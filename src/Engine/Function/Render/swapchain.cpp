#include "swapchain.h"

#include "Function/Render/context.h"

#include <VKBootstrap.h>

namespace Yutrel
{
    Swapchain::Swapchain(const SwapchianCreateInfo& info)
        : m_context(info.context)
    {
        init(info);
    }

    Swapchain::~Swapchain()
    {
        destroy();
    }

    void Swapchain::init(const SwapchianCreateInfo& info)
    {
        vkb::SwapchainBuilder swapchain_builder(m_context->getGPU(), m_context->getDevice(), m_context->getSurface());

        vkb::Swapchain vkb_swapchain =
            swapchain_builder
                .use_default_format_selection()
                .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                .set_desired_extent(info.width, info.height)
                .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build()
                .value();

        // 获取交换链和图像
        m_swapchain   = vkb_swapchain.swapchain;
        m_extent      = vkb_swapchain.extent;
        m_format      = static_cast<vk::Format>(vkb_swapchain.image_format);
        m_image_count = vkb_swapchain.image_count;
        m_images.clear();
        m_image_views.clear();
        auto swapchain_images      = vkb_swapchain.get_images().value();
        auto swapchain_image_views = vkb_swapchain.get_image_views().value();
        for (auto image : swapchain_images)
        {
            m_images.push_back(static_cast<vk::Image>(image));
        }
        for (auto&& view : swapchain_image_views)
        {
            m_image_views.push_back(static_cast<vk::ImageView>(view));
        }
    }

    void Swapchain::destroy()
    {
        auto device = m_context->getDevice();

        device.destroy(m_swapchain);

        for (auto view : m_image_views)
        {
            device.destroy(view);
        }
    }

    void Swapchain::acquireNextImage(vk::Semaphore semaphore)
    {
        auto device = m_context->getDevice();

        // todo resize
        auto result = device.acquireNextImageKHR(m_swapchain, UINT64_MAX, semaphore, {});

        m_cur_image_index = result.value;
    }

    vk::Result Swapchain::present(vk::Semaphore semaphore)
    {
        auto queue = m_context->getGraphicsQueue();

        auto present_info =
            vk::PresentInfoKHR()
                .setSwapchains(m_swapchain)
                .setWaitSemaphores(semaphore)
                .setImageIndices(m_cur_image_index);

        return queue.presentKHR(present_info);
    }

} // namespace Yutrel