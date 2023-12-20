#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    namespace vkinit
    {
        // 指令池创建信息
        VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags flags);

        // 指令缓冲分配信息
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level);

        // 栅栏创建信息
        VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags);

        // 信号量创建信息
        VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags);

        // 帧缓冲创建信息
        VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass render_pass, VkExtent2D extent);
        
    } // namespace vkinit

} // namespace Yutrel