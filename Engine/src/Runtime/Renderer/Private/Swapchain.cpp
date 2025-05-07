#include "Swapchain.h"

#include "Context.h"
#include "Renderer.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <VKBootstrap.h>

namespace Yutrel
{
    Swapchain::Swapchain(const CreateInfo& info)
        : m_context(info.renderer->getContext())
    {
        init(info);
    }

    Swapchain::~Swapchain()
    {
        destroy();
    }

    void Swapchain::init(const CreateInfo& info)
    {
        // surface
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(m_context->getInstance(), info.window->getWindow(), nullptr, &surface);
        m_surface = static_cast<vk::SurfaceKHR>(surface);

        vkb::SwapchainBuilder swapchain_builder(m_context->getGPU(), m_context->getDevice(), m_surface);

        vkb::Swapchain vkb_swapchain =
            swapchain_builder
                .set_desired_format({VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                .set_desired_extent(info.window->getWidth(), info.window->getHeight())
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

        device.waitIdle();

        for (auto view : m_image_views)
        {
            device.destroy(view);
        }

        device.destroy(m_swapchain);

        m_context->getInstance().destroy(m_surface);
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