#pragma once

#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace Yutrel
{
    class Swapchain final
    {
    public:
        struct CreateInfo
        {
            std::shared_ptr<class Renderer> renderer;
            std::shared_ptr<class Window> window;
        };

    public:
        Swapchain() = delete;
        explicit Swapchain(const CreateInfo& info);
        ~Swapchain();

        Swapchain(const Swapchain&)            = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        void acquireNextImage(vk::Semaphore semaphore);
        vk::Result present(vk::Semaphore semaphore);

        vk::SwapchainKHR getSwapchain() const { return m_swapchain; }
        vk::Image getCurrentImage() const { return m_images[m_cur_image_index]; }

    private:
        void init(const CreateInfo& info);
        void destroy();

    private:
        vk::SurfaceKHR m_surface{nullptr};
        vk::SwapchainKHR m_swapchain{nullptr};
        vk::Format m_format;
        vk::Extent2D m_extent;
        std::vector<vk::Image> m_images;
        std::vector<vk::ImageView> m_image_views;
        uint32_t m_image_count{0};
        uint32_t m_cur_image_index{0};

        std::shared_ptr<class Context> m_context;
    };

} // namespace Yutrel