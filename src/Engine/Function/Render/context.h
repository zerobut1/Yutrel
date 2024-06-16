#pragma once

#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace Yutrel
{
    struct ContextCreateInfo
    {
        bool enable_validation_layers{true};

        GLFWwindow* window{nullptr};

        vk::PhysicalDeviceFeatures device_features{};
        vk::PhysicalDeviceVulkan12Features device_features_12{};
        vk::PhysicalDeviceVulkan13Features device_features_13{};
    };

    class Context
    {
    public:
        Context() = delete;
        Context(const ContextCreateInfo& info);
        ~Context();

        Context(const Context&)            = delete;
        Context& operator=(const Context&) = delete;

        vk::PhysicalDevice getGPU() const { return m_GPU; }
        vk::Device getDevice() const { return m_device; }
        vk::Instance getInstance() const { return m_instance; }
        vk::SurfaceKHR getSurface() const { return m_surface; }
        vk::Queue getGraphicsQueue() const { return m_graphics_queue; }
        uint32_t getGraphicsQueueIndex() const { return m_graphics_queue_family_index; }

    private:
        void init(const ContextCreateInfo& info);
        void destroy();

    private:
        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debug_messenger;
        vk::SurfaceKHR m_surface;
        vk::PhysicalDevice m_GPU;
        vk::PhysicalDeviceProperties m_GPU_properties;
        vk::Device m_device;
        vk::Queue m_graphics_queue;
        uint32_t m_graphics_queue_family_index;
    };
} // namespace Yutrel