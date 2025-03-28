#pragma once

#include "Window.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <string>
#include <vector>

namespace Yutrel
{
    struct ApplicationCreateInfo
    {
        std::string name{"Yutrel App"};
        uint32_t width{1920};
        uint32_t height{1080};
    };

    class ComponentBase;

    class Application final : public Window::ICallbacks
    {
    public:
        Application() = delete;
        explicit Application(const ApplicationCreateInfo& info);
        ~Application();

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

    public:
        void run();

        void addComponent(const std::shared_ptr<ComponentBase>& component);

        std::shared_ptr<class Renderer> getRenderer() const { return m_renderer; }
        std::shared_ptr<class Window> getWindow() const { return m_window; }
        std::shared_ptr<class Swapchain> getSwapchain() const { return m_swapchain; }

    private:
        void init(const ApplicationCreateInfo& info);
        void shutdown();

    private:
        void handleWindowSizeChange() override;

    private:
        std::vector<std::shared_ptr<ComponentBase>> m_components;

        std::shared_ptr<Renderer> m_renderer;
        std::shared_ptr<Window> m_window;
        std::shared_ptr<Swapchain> m_swapchain;

        uint32_t m_viewport_width{0};
        uint32_t m_viewport_height{0};
    };

    class ComponentBase
    {
    public:
        virtual ~ComponentBase() = default;

        virtual void onAttach(Application* app)                = 0;
        virtual void onDetach()                                = 0;
        virtual void onRender(vk::CommandBuffer cmd_buffer)    = 0;
        virtual void onResize(uint32_t width, uint32_t height) = 0;
    };

} // namespace Yutrel