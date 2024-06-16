#include "frame.h"

#include "Core/log.h"
#include "Function/Render/context.h"

namespace Yutrel
{
    Frame::Frame(std::shared_ptr<Context> context)
        : m_context(context)
    {
        init();
    };

    Frame::~Frame()
    {
        destroy();
    }

    void Frame::init()
    {
        auto device = m_context->getDevice();

        // command
        auto cmd_pool_ci =
            vk::CommandPoolCreateInfo()
                .setQueueFamilyIndex(m_context->getGraphicsQueueIndex())
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_cmd_pool = device.createCommandPool(cmd_pool_ci);

        auto cmd_buffer_ai =
            vk::CommandBufferAllocateInfo()
                .setCommandPool(m_cmd_pool)
                .setCommandBufferCount(1)
                .setLevel(vk::CommandBufferLevel::ePrimary);

        m_cmd_buffer = device.allocateCommandBuffers(cmd_buffer_ai).front();

        // 同步
        auto fence_ci =
            vk::FenceCreateInfo()
                .setFlags(vk::FenceCreateFlagBits::eSignaled);
        m_render_fence = device.createFence(fence_ci);

        auto semaphore_ci =
            vk::SemaphoreCreateInfo()
                .setFlags({});
        m_finished_for_presentation_semaphore = device.createSemaphore(semaphore_ci);
        m_available_for_render_semaphore      = device.createSemaphore(semaphore_ci);
    }

    void Frame::destroy()
    {
        auto device = m_context->getDevice();

        device.destroy(m_cmd_pool);

        device.destroy(m_render_fence);

        device.destroy(m_finished_for_presentation_semaphore);
        device.destroy(m_available_for_render_semaphore);
    }

    void Frame::waitForFence() const
    {
        auto device = m_context->getDevice();

        YUTREL_ASSERT(device.waitForFences(m_render_fence, vk::True, UINT64_MAX) == vk::Result::eSuccess, "Failed to synchronize");
        device.resetFences(m_render_fence);
    }

    vk::CommandBuffer Frame::beginCommandBuffer() const
    {
        auto device = m_context->getDevice();

        device.resetCommandPool(m_cmd_pool);

        auto cmd_buffer_bi =
            vk::CommandBufferBeginInfo()
                .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_cmd_buffer.begin(cmd_buffer_bi);

        return m_cmd_buffer;
    }

    void Frame::endCommandBuffer() const
    {
        m_cmd_buffer.end();
    }

    void Frame::submitCommandBuffer() const
    {
        auto cmd_buffer_si =
            vk::CommandBufferSubmitInfo()
                .setCommandBuffer(m_cmd_buffer);

        auto semaphore_wait_si =
            vk::SemaphoreSubmitInfo()
                .setSemaphore(m_available_for_render_semaphore)
                .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
                .setDeviceIndex(0)
                .setValue(1);

        auto semaphore_signal_si =
            vk::SemaphoreSubmitInfo()
                .setSemaphore(m_finished_for_presentation_semaphore)
                .setStageMask(vk::PipelineStageFlagBits2::eAllGraphics)
                .setDeviceIndex(0)
                .setValue(1);

        auto submit_info =
            vk::SubmitInfo2()
                .setWaitSemaphoreInfos(semaphore_wait_si)
                .setSignalSemaphoreInfos(semaphore_signal_si)
                .setCommandBufferInfos(cmd_buffer_si);

        // 提交到队列
        m_context->getGraphicsQueue().submit2(submit_info, m_render_fence);
    }

} // namespace Yutrel