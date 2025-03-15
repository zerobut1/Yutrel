#pragma once

#include <vulkan/vulkan.hpp>

namespace Yutrel
{
    class Frame final
    {
    public:
        Frame() = delete;
        Frame(std::shared_ptr<class Context> context);
        ~Frame();

        Frame(const Frame&)            = delete;
        Frame& operator=(const Frame&) = delete;

        void waitForFence() const;
        vk::CommandBuffer beginCommandBuffer() const;
        void endCommandBuffer() const;
        void submitCommandBuffer() const;

        vk::Semaphore getAvailableForRenderSemaphore() const { return m_available_for_render_semaphore; }
        vk::Semaphore getFinishedForPresentationSemaphore() const { return m_finished_for_presentation_semaphore; }

    private:
        void init();
        void destroy();

    private:
        std::shared_ptr<Context> m_context;

        // 指令
        vk::CommandPool m_cmd_pool;
        vk::CommandBuffer m_cmd_buffer;

        // 同步
        vk::Semaphore m_finished_for_presentation_semaphore, m_available_for_render_semaphore;
        vk::Fence m_render_fence;
    };
} // namespace Yutrel