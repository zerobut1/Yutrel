#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace Yutrel
{
    struct RendererCreateInfo
    {
        GLFWwindow* window;
        uint32_t width;
        uint32_t height;
        vk::PhysicalDeviceFeatures device_features{};
        vk::PhysicalDeviceVulkan12Features device_features_12{};
        vk::PhysicalDeviceVulkan13Features device_features_13{};
    };

    class Renderer final
    {
    public:
        Renderer() = delete;
        explicit Renderer(const RendererCreateInfo& info);
        ~Renderer();

        Renderer(const Renderer&)            = delete;
        Renderer& operator=(const Renderer&) = delete;

        std::shared_ptr<class Swapchain> getSwapchain() const { return m_swapchain; }

        vk::CommandBuffer prepareBeforeRender();
        void submitRendering();
        void framePresent();

        void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout);

    private:
        void init(const RendererCreateInfo& info);
        void shutdown();

        std::shared_ptr<class Frame> getCurrentFrame() const { return m_frames[m_frame_count % s_max_frame]; }

    private:
        static constexpr uint8_t s_max_frame{2};

        std::shared_ptr<class Context> m_context;
        std::shared_ptr<Swapchain> m_swapchain;
        vma::Allocator m_allocator{nullptr};

        uint32_t m_frame_count{0};
        std::array<std::shared_ptr<Frame>, s_max_frame> m_frames;
        vk::CommandPool m_cmd_pool{nullptr};
    };

} // namespace Yutrel