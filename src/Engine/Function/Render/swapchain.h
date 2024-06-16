#pragma once

#include <vulkan/vulkan.hpp>

namespace Yutrel
{
    struct SwapchianCreateInfo
    {
        std::shared_ptr<class Context> context;
        uint32_t width;
        uint32_t height;
    };

    class Swapchain final
    {
    public:
        Swapchain() = delete;
        explicit Swapchain(const SwapchianCreateInfo& info);
        ~Swapchain();

        Swapchain(const Swapchain&)            = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        void acquireNextImage(vk::Semaphore semaphore);
        vk::Result present(vk::Semaphore semaphore);

        vk::SwapchainKHR getSwapchain() const { return m_swapchain; }
        vk::Image getCurrentImage() const { return m_images[m_cur_image_index]; }

    private:
        void init(const SwapchianCreateInfo& info);
        void destroy();

    private:
        vk::SwapchainKHR m_swapchain{nullptr};
        vk::Format m_format;
        vk::Extent2D m_extent;
        std::vector<vk::Image> m_images;
        std::vector<vk::ImageView> m_image_views;
        uint32_t m_image_count{0};
        uint32_t m_cur_image_index{0};

        std::shared_ptr<Context> m_context;
    };

} // namespace Yutrel