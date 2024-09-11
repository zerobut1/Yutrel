#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace Yutrel
{

    class Renderer final
    {
    public:
        struct CreateInfo
        {
            vk::PhysicalDeviceFeatures device_features{};
            vk::PhysicalDeviceVulkan12Features device_features_12{};
            vk::PhysicalDeviceVulkan13Features device_features_13{};
        };

    public:
        Renderer() = delete;
        explicit Renderer(const CreateInfo& info);
        ~Renderer();

        Renderer(const Renderer&)            = delete;
        Renderer& operator=(const Renderer&) = delete;

    public:
        std::shared_ptr<class Context> getContext() { return m_context; }

        vk::CommandBuffer prepareBeforeRender();
        void submitRendering();
        void framePresent();

        void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout);

    private:
        void init(const CreateInfo& info);
        void shutdown();

        std::shared_ptr<class Frame> getCurrentFrame() const { return m_frames[m_frame_count % s_max_frame]; }

    private:
        static constexpr uint8_t s_max_frame{2};

        std::shared_ptr<Context> m_context;
        std::shared_ptr<class ResourceManager> m_resource_manager;

        uint32_t m_frame_count{0};
        std::array<std::shared_ptr<Frame>, s_max_frame> m_frames;
        vk::CommandPool m_cmd_pool{nullptr};
    };

} // namespace Yutrel