#pragma once

#include <vulkan/vulkan.hpp>

#include <functional>

namespace Yutrel
{
    class Renderer;
    class Window;
    class Swapchain;

    class Gui
    {
    public:
        struct CreateInfo
        {
            Renderer* renderer;
            Window* window;
            Swapchain* swapchain;
        };

        Gui() = delete;
        explicit Gui(const CreateInfo& info);
        ~Gui();

        void updateUI(const std::function<void()>& onUIUpdate);
        void drawUI(vk::CommandBuffer cmd_buffer);

    private:
        void init(const CreateInfo& info);
        void shutdown();

    private:
        Renderer* m_renderer;
        Window* m_window;
        Swapchain* m_swapchain;

        vk::DescriptorPool m_descriptor_pool{nullptr};
    };
} // namespace Yutrel